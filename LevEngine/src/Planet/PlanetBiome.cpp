#include "levpch.h"
#include "PlanetBiome.h"

namespace LevEngine
{
	namespace
	{
		// Membership of one window: 1 well inside it, 0 well outside, and a smooth ramp of the given
		// width at each edge. Windows narrower than twice their blend never reach 1, which is
		// deliberate -- a biome squeezed into a range too narrow to fade across should lose to its
		// neighbours rather than win a sliver of ground outright.
		float Window(const float value, const float min, const float max, const float blend)
		{
			if (blend <= 0.0f)
				return value >= min && value <= max ? 1.0f : 0.0f;

			const float rise = Math::Smoothstep(min, min + blend, value);
			const float fall = 1.0f - Math::Smoothstep(max - blend, max, value);

			return rise * fall;
		}
	}

	float PlanetBiome::GetWeight(const float temperature, const float humidity, const float height,
	                             const float slope) const
	{
		// A product, not a sum: every condition is necessary. A place that is warm enough for jungle
		// but has no rain is not partly jungle, it is desert, and only multiplying gets that right.
		float weight = Window(temperature, MinTemperature, MaxTemperature, TemperatureBlend);
		if (weight <= 0.0f) return 0.0f;

		weight *= Window(humidity, MinHumidity, MaxHumidity, HumidityBlend);
		if (weight <= 0.0f) return 0.0f;

		weight *= Window(height, MinHeight, MaxHeight, HeightBlend);
		if (weight <= 0.0f) return 0.0f;

		weight *= Window(slope, MinSlope, MaxSlope, SlopeBlend);

		return weight * Math::Max(0.0f, Priority);
	}

	PlanetBiomeBlend PlanetBiomeTable::Classify(const Vector<PlanetBiome>& biomes, const float temperature,
	                                            const float humidity, const float height, const float slope)
	{
		PlanetBiomeBlend blend;

		const auto count = static_cast<int32_t>(Math::Min<size_t>(biomes.size(), k_MaxPlanetBiomes));

		for (int32_t index = 0; index < count; ++index)
		{
			const float weight = biomes[index].GetWeight(temperature, humidity, height, slope);
			if (weight <= 0.0f) continue;

			// Insertion into a sorted list of four. Four is small enough that this beats collecting
			// everything and sorting, and it means the loop never allocates.
			for (uint32_t slot = 0; slot < k_MaxBlendedBiomes; ++slot)
			{
				if (slot < blend.Count && weight <= blend.Weights[slot]) continue;

				for (uint32_t shift = k_MaxBlendedBiomes - 1; shift > slot; --shift)
				{
					blend.Weights[shift] = blend.Weights[shift - 1];
					blend.Indices[shift] = blend.Indices[shift - 1];
				}

				blend.Weights[slot] = weight;
				blend.Indices[slot] = index;

				if (blend.Count < k_MaxBlendedBiomes)
					++blend.Count;

				break;
			}
		}

		float total = 0.0f;
		for (uint32_t slot = 0; slot < blend.Count; ++slot)
			total += blend.Weights[slot];

		if (total <= 0.0f)
		{
			//<--- Nothing matched: the caller shows the first biome rather than nothing at all ---<<
			blend.Count = 0;
			return blend;
		}

		const float normalize = 1.0f / total;
		for (uint32_t slot = 0; slot < blend.Count; ++slot)
			blend.Weights[slot] *= normalize;

		return blend;
	}

	int32_t PlanetBiomeTable::ClassifyDominant(const Vector<PlanetBiome>& biomes, const float temperature,
	                                           const float humidity, const float height, const float slope)
	{
		const PlanetBiomeBlend blend = Classify(biomes, temperature, humidity, height, slope);

		return blend.Count > 0 ? blend.Indices[0] : 0;
	}

	Vector<PlanetBiome> PlanetBiomeTable::CreateEarthlikeSet()
	{
		// The Whittaker diagram, plus the two things it leaves out: height, which decides sea floor
		// from shore from alpine, and slope, which decides whether anything grows at all.
		//
		// Heights suit the default shape settings -- 220 units of ocean, 90 of land, 380 of mountain.
		// A planet with a different vertical scale wants these scaled with it, which is most of what a
		// biome set is for.
		//
		// Only the ones that have to be are height gated. A rainforest does not need a ceiling: three
		// kilometres up it is too cold for one, and the lapse rate has already said so. Height is used
		// for the things climate genuinely cannot tell apart -- the sea floor from the shore, and bare
		// alpine rock from the tundra below it.
		Vector<PlanetBiome> biomes;
		biomes.reserve(k_MaxPlanetBiomes);

		auto add = [&biomes](const char* name, const float minTemperature, const float maxTemperature,
		                     const float minHumidity, const float maxHumidity,
		                     const float minHeight, const float maxHeight,
		                     const float maxSlope, const Color tint, const float roughness,
		                     const float textureScale, const int32_t textureIndex, const float priority)
		{
			PlanetBiome biome;
			biome.Name = name;
			biome.MinTemperature = minTemperature;
			biome.MaxTemperature = maxTemperature;
			biome.MinHumidity = minHumidity;
			biome.MaxHumidity = maxHumidity;
			biome.MinHeight = minHeight;
			biome.MaxHeight = maxHeight;
			biome.MaxSlope = maxSlope;
			biome.Tint = tint;
			biome.Roughness = roughness;
			biome.TextureScale = textureScale;
			biome.TextureIndex = textureIndex;
			biome.Priority = priority;

			biomes.emplace_back(biome);
		};

		//<--- Under water. Climate does not reach here, only depth ---<<
		add("Abyssal Floor", -80, 80, 0, 1, -100000, -110, 1.0f, Color(0.16f, 0.15f, 0.14f, 1), 0.95f, 40, 0, 1);
		add("Sea Floor", -80, 80, 0, 1, -125, -4, 1.0f, Color(0.55f, 0.5f, 0.38f, 1), 0.9f, 14, 1, 1);

		// The shore. Its height window is what makes a beach a beach: a narrow band around the
		// waterline, which exists only because the shape spends ShoreWidth of continent field
		// climbing out of the sea. A cold coast gets shingle instead of sand.
		add("Beach", 2, 80, 0, 1, -5, 14, 0.35f, Color(0.86f, 0.8f, 0.62f, 1), 0.85f, 8, 2, 1.6f);
		biomes.back().HeightBlend = 5.0f;

		add("Rocky Shore", -80, 4, 0, 1, -5, 16, 0.6f, Color(0.42f, 0.42f, 0.44f, 1), 0.8f, 10, 3, 1.6f);
		biomes.back().HeightBlend = 5.0f;

		//<--- Warm ---<<
		add("Rainforest", 21, 60, 0.62f, 1.0f, 0, 100000, 0.55f, Color(0.13f, 0.32f, 0.11f, 1), 0.95f, 16, 4, 1);
		add("Savanna", 19, 60, 0.26f, 0.66f, 0, 100000, 0.55f, Color(0.55f, 0.5f, 0.24f, 1), 0.9f, 14, 5, 1);
		add("Desert", 13, 60, 0.0f, 0.28f, 0, 100000, 0.5f, Color(0.78f, 0.66f, 0.44f, 1), 0.85f, 10, 6, 1);

		//<--- Temperate: the fields and the woods ---<<
		add("Grassland", 5, 23, 0.28f, 0.68f, 0, 100000, 0.5f, Color(0.34f, 0.46f, 0.19f, 1), 0.9f, 12, 7, 1);
		add("Temperate Forest", 4, 22, 0.62f, 1.0f, 0, 100000, 0.55f, Color(0.19f, 0.34f, 0.15f, 1), 0.95f, 15, 8, 1);
		add("Wetland", 4, 30, 0.82f, 1.0f, 0, 28, 0.15f, Color(0.24f, 0.31f, 0.17f, 1), 0.98f, 11, 9, 1.3f);
		biomes.back().HeightBlend = 10.0f;

		add("Steppe", -6, 16, 0.0f, 0.26f, 0, 100000, 0.5f, Color(0.6f, 0.55f, 0.35f, 1), 0.88f, 13, 10, 1);

		//<--- Cold ---<<
		add("Taiga", -9, 7, 0.32f, 1.0f, 0, 100000, 0.55f, Color(0.17f, 0.27f, 0.19f, 1), 0.93f, 14, 11, 1);
		add("Tundra", -22, 1, 0.0f, 0.55f, 0, 100000, 0.55f, Color(0.42f, 0.42f, 0.33f, 1), 0.9f, 12, 12, 1);
		add("Snow", -80, -5, 0.0f, 1.0f, 0, 100000, 0.7f, Color(0.92f, 0.94f, 0.97f, 1), 0.4f, 18, 13, 1);

		// Rock, which is what is left where nothing grows. Both entries carry a priority above the
		// climate biomes: a cliff face and the meadow at its foot have the same temperature and the
		// same rainfall, and only the slope tells them apart -- so the slope has to win.
		add("Cliff", -80, 80, 0.0f, 1.0f, -100000, 100000, 1.0f, Color(0.38f, 0.36f, 0.34f, 1), 0.85f, 16, 14, 1.4f);
		biomes.back().MinSlope = 0.42f;
		biomes.back().SlopeBlend = 0.14f;

		add("Alpine Rock", -80, 6, 0.0f, 1.0f, 200, 100000, 1.0f, Color(0.45f, 0.43f, 0.4f, 1), 0.88f, 16, 15, 1.2f);
		biomes.back().HeightBlend = 60.0f;

		return biomes;
	}
}
