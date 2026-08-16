#include "pch.h"

#include "ComponentDrawer.h"
#include "Assets/PlanetBiomeSetAsset.h"
#include "Planet/PlanetSurface.h"
#include "Scene/Components/Planet/Planet.h"

namespace LevEngine::Editor
{
	namespace
	{
		// Every fractal has the same four knobs, and a planet has six of them. Drawn as a collapsing
		// header each, because the interesting ones are the amplitudes next to them and these are
		// what somebody opens once to make a world rougher.
		bool DrawFractal(const char* label, Noise::FractalSettings& settings)
		{
			bool edited = false;

			if (!ImGui::TreeNode(label)) return false;

			int octaves = settings.Octaves;

			edited |= ImGui::DragFloat("Frequency", &settings.Frequency, 0.05f, 0.05f, 200.0f);

			if (ImGui::DragInt("Octaves", &octaves, 1, 1, 12))
			{
				settings.Octaves = Math::Clamp(octaves, 1, 12);
				edited = true;
			}

			edited |= ImGui::DragFloat("Lacunarity", &settings.Lacunarity, 0.01f, 1.01f, 4.0f);
			edited |= ImGui::DragFloat("Gain", &settings.Gain, 0.01f, 0.05f, 0.95f);

			ImGui::TreePop();

			return edited;
		}
	}

	class PlanetDrawer final : public ComponentDrawer<PlanetComponent, PlanetDrawer>
	{
	protected:
		String GetLabel() const override { return "Environment/Planet"; }

		void DrawContent(PlanetComponent& component) override
		{
			const char* presetNames[] = {"Earthlike", "Desert", "Ice World", "Ocean World", "Volcanic", "Custom"};

			int preset = static_cast<int>(component.Preset);
			if (ImGui::Combo("Preset", &preset, presetNames, IM_ARRAYSIZE(presetNames)))
				PlanetPresets::Apply(component, static_cast<PlanetPreset>(preset));

			ImGui::TextWrapped(
				"Six cube faces subdivided towards the camera. Heights are world units and are measured "
				"from sea level, so the numbers below are the same ones the biome heights are in. Add an "
				"Atmosphere with a matching Planet Radius to give it a sky.");

			// Anything typed by hand stops being one of the presets, the same as the atmosphere.
			bool edited = false;

			//<--- Shape ---<<

			ImGui::SeparatorText("Shape");

			int seed = static_cast<int>(component.Shape.Seed);
			if (ImGui::DragInt("Seed", &seed))
			{
				component.Shape.Seed = static_cast<uint32_t>(Math::Max(0, seed));
				edited = true;
			}

			ImGui::SameLine();
			if (ImGui::Button("Randomize"))
			{
				component.Shape.Seed = static_cast<uint32_t>(Random::Int(0, 1000000));
				edited = true;
			}

			edited |= ImGui::DragFloat("Radius", &component.Shape.Radius, 10.0f, 10.0f, 10000000.0f);

			edited |= ImGui::DragFloat("Sea Level", &component.Shape.SeaLevelThreshold, 0.01f, -1.0f, 1.0f);
			ImGui::SetItemTooltip("Where the shoreline sits in the continent field: the land to water "
			                      "ratio and nothing else. Earth is around 0.25.");

			edited |= ImGui::DragFloat("Shore Width", &component.Shape.ShoreWidth, 0.005f, 0.001f, 0.6f);
			ImGui::SetItemTooltip("How much of the continent field is spent climbing out of the sea. "
			                      "This is what decides whether there is anywhere for a beach to be.");

			edited |= ImGui::DragFloat("Ocean Depth", &component.Shape.OceanDepth, 5.0f, 0.0f, 100000.0f);
			edited |= ImGui::DragFloat("Land Height", &component.Shape.LandHeight, 5.0f, 0.0f, 100000.0f);
			edited |= ImGui::DragFloat("Mountain Height", &component.Shape.MountainHeight, 10.0f, 0.0f, 100000.0f);
			edited |= ImGui::DragFloat("Detail Height", &component.Shape.DetailHeight, 1.0f, 0.0f, 10000.0f);

			if (ImGui::TreeNode("Shape detail"))
			{
				edited |= ImGui::DragFloat("Continent Warp", &component.Shape.ContinentWarp, 0.01f, 0.0f, 4.0f);
				ImGui::SetItemTooltip("Folds the continent field before it is read, which is what turns "
				                      "round blobs into bays, headlands and island chains.");

				edited |= ImGui::DragFloat("Warp Frequency", &component.Shape.ContinentWarpFrequency,
				                           0.05f, 0.05f, 20.0f);

				edited |= ImGui::DragFloat("Mountain Belt Width", &component.Shape.MountainBeltWidth,
				                           0.01f, 0.0f, 1.0f);
				ImGui::SetItemTooltip("Ranges run in belts rather than covering a continent. This is how "
				                      "wide those belts are.");

				edited |= ImGui::DragFloat("Mountain Inland Bias", &component.Shape.MountainInlandBias,
				                           0.01f, 0.0f, 1.0f);
				edited |= ImGui::DragFloat("Detail Relief Bias", &component.Shape.DetailReliefBias,
				                           0.01f, 0.0f, 1.0f);
				ImGui::SetItemTooltip("Keeps the fine roughness on the mountains, where it is scree, and "
				                      "off the plains, where it is sandpaper.");

				edited |= DrawFractal("Continents", component.Shape.Continents);
				edited |= DrawFractal("Mountains", component.Shape.Mountains);
				edited |= DrawFractal("Mountain Belts", component.Shape.MountainBelts);
				edited |= DrawFractal("Surface Detail", component.Shape.Detail);

				ImGui::TreePop();
			}

			//<--- Climate ---<<

			ImGui::SeparatorText("Climate");
			ImGui::TextWrapped(
				"Biomes are read out of temperature and rainfall rather than painted on, so snow appears "
				"where it is cold and desert where the rain does not reach.");

			edited |= ImGui::DragFloat("Equator Temperature", &component.Climate.EquatorTemperature,
			                           0.5f, -150.0f, 200.0f, "%.1f C");
			edited |= ImGui::DragFloat("Pole Temperature", &component.Climate.PoleTemperature,
			                           0.5f, -150.0f, 200.0f, "%.1f C");
			edited |= ImGui::DragFloat("Lapse Rate", &component.Climate.LapseRate, 0.1f, 0.0f, 30.0f, "%.1f C/km");
			ImGui::SetItemTooltip("Degrees lost per kilometre of altitude. This is why a mountain on the "
			                      "equator has snow on it. Earth averages 6.5.");

			edited |= ImGui::DragFloat("Kilometers Per Unit", &component.Climate.KilometersPerUnit,
			                           0.0001f, 0.000001f, 10.0f, "%.5f");

			edited |= ImGui::DragFloat("Humidity At Coast", &component.Climate.HumidityAtCoast, 0.01f, 0.0f, 1.0f);
			edited |= ImGui::DragFloat("Humidity Inland", &component.Climate.HumidityInland, 0.01f, 0.0f, 1.0f);

			edited |= ImGui::DragFloat("Circulation", &component.Climate.CirculationStrength, 0.01f, 0.0f, 1.0f);
			ImGui::SetItemTooltip("The banded circulation: wet at the equator, dry around thirty degrees, "
			                      "wet again around sixty. This is why deserts sit in belts.");

			edited |= ImGui::DragFloat("Rain Shadow", &component.Climate.RainShadowStrength, 0.01f, 0.0f, 1.0f);
			ImGui::SetItemTooltip("How much a range takes out of the air crossing it. Costs one extra "
			                      "elevation sample per vertex, which is most of the generation time.");

			if (ImGui::TreeNode("Climate detail"))
			{
				edited |= ImGui::DragFloat("Latitude Falloff", &component.Climate.LatitudeFalloff,
				                           0.05f, 0.1f, 8.0f);
				edited |= ImGui::DragFloat("Temperature Variation", &component.Climate.TemperatureVariationRange,
				                           0.1f, 0.0f, 40.0f);
				edited |= ImGui::DragFloat("Humidity Variation", &component.Climate.HumidityVariationRange,
				                           0.01f, 0.0f, 1.0f);
				edited |= ImGui::DragFloat("Rain Shadow Arc", &component.Climate.RainShadowArc,
				                           0.1f, 0.0f, 45.0f, "%.1f deg");
				edited |= ImGui::DragFloat("Rain Shadow Relief", &component.Climate.RainShadowRelief,
				                           5.0f, 1.0f, 100000.0f);
				edited |= ImGui::DragFloat("Altitude Drying", &component.Climate.AltitudeDrying,
				                           0.01f, 0.0f, 1.0f);

				int climateSeed = static_cast<int>(component.Climate.Seed);
				if (ImGui::DragInt("Climate Seed", &climateSeed))
				{
					component.Climate.Seed = static_cast<uint32_t>(Math::Max(0, climateSeed));
					edited = true;
				}

				edited |= DrawFractal("Temperature Field", component.Climate.TemperatureVariation);
				edited |= DrawFractal("Humidity Field", component.Climate.HumidityVariation);

				ImGui::TreePop();
			}

			//<--- Biomes ---<<

			ImGui::SeparatorText("Biomes");

			if (EditorGUI::DrawAsset<PlanetBiomeSetAsset>("Biome Set", component.BiomeSet))
			{
				//<--- Only what is drawn changes, so nothing has to be regenerated ---<<
			}

			if (!component.BiomeSet)
			{
				ImGui::TextWrapped(
					"Without a set the default Earthlike biomes are used, drawn with their tint colours "
					"and no textures. Create one from the asset browser to assign ground textures.");
			}

			//<--- Level of detail ---<<

			ImGui::SeparatorText("Level Of Detail");

			int resolution = static_cast<int>(component.Lod.ChunkResolution);
			if (ImGui::DragInt("Chunk Resolution", &resolution, 1, 4, 128))
			{
				component.Lod.ChunkResolution = static_cast<uint32_t>(Math::Clamp(resolution, 4, 128));
				edited = true;
			}
			ImGui::SetItemTooltip("Quads per side of a chunk. Larger chunks are cheaper to submit and "
			                      "slower to react to the camera, since a chunk is the unit rebuilt.");

			int maxDepth = static_cast<int>(component.Lod.MaxDepth);
			if (ImGui::DragInt("Max Depth", &maxDepth, 1, 0, 16))
			{
				component.Lod.MaxDepth = static_cast<uint32_t>(Math::Clamp(maxDepth, 0, 16));
				edited = true;
			}

			ImGui::DragFloat("Triangle Pixels", &component.Lod.TargetTrianglePixels, 0.1f, 0.5f, 32.0f);
			ImGui::SetItemTooltip("How large a chunk's triangles may get on screen before it splits. "
			                      "Lower is finer, and the triangle count climbs as the square of it. "
			                      "3 is about where a silhouette stops looking faceted.");

			ImGui::DragFloat("Skirt Depth", &component.Lod.SkirtDepthScale, 0.005f, 0.0f, 1.0f);
			ImGui::SetItemTooltip("Depth of the wall around each chunk that hides the crack between two "
			                      "levels of detail, as a fraction of the chunk's width.");

			ImGui::DragFloat("Merge Hysteresis", &component.Lod.MergeHysteresis, 0.01f, 0.05f, 1.0f);
			ImGui::SetItemTooltip("How far below the split threshold a chunk falls before its children "
			                      "are given back. 1 means it merges the moment it stops wanting to "
			                      "split, which makes a chunk on the boundary rebuild its children "
			                      "every frame while the camera drifts.");

			int uploads = static_cast<int>(component.Lod.MaxMeshUploadsPerFrame);
			if (ImGui::DragInt("Mesh Uploads / Frame", &uploads, 1, 1, 32))
				component.Lod.MaxMeshUploadsPerFrame = static_cast<uint32_t>(Math::Clamp(uploads, 1, 32));

			ImGui::SetItemTooltip("Chunks whose buffers may be created in one frame. Creating them is a "
			                      "driver allocation on this thread, so a burst of finished builds -- "
			                      "which is what starting to move produces -- shows up as a hitch.");

			int builds = static_cast<int>(component.Lod.MaxConcurrentBuilds);
			if (ImGui::DragInt("Concurrent Builds", &builds, 1, 1, 64))
				component.Lod.MaxConcurrentBuilds = static_cast<uint32_t>(Math::Clamp(builds, 1, 64));

			//<--- Ocean ---<<

			ImGui::SeparatorText("Ocean");

			ImGui::Checkbox("Render Ocean", &component.Ocean.Render);

			if (component.Ocean.Render)
			{
				ImGui::ColorEdit3("Shallow Color", component.Ocean.ShallowColor.Raw());
				ImGui::ColorEdit3("Deep Color", component.Ocean.DeepColor.Raw());
				ImGui::DragFloat("Depth Falloff", &component.Ocean.DepthFalloff, 1.0f, 1.0f, 100000.0f);
				ImGui::SetItemTooltip("Depth at which the water reads as fully deep. Water gets its "
				                      "colour from how much of it the light goes through.");

				ImGui::DragFloat("Water Roughness", &component.Ocean.Roughness, 0.005f, 0.01f, 1.0f);
				ImGui::DragFloat("Opacity", &component.Ocean.Opacity, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Fresnel", &component.Ocean.FresnelStrength, 0.02f, 0.0f, 4.0f);
				ImGui::DragFloat("Wave Strength", &component.Ocean.WaveStrength, 0.01f, 0.0f, 4.0f);
				ImGui::DragFloat("Wave Scale", &component.Ocean.WaveScale, 0.01f, 0.001f, 20.0f);
				ImGui::DragFloat("Wave Speed", &component.Ocean.WaveSpeed, 0.01f, 0.0f, 10.0f);
			}

			//<--- Surface detail, which is shader side and needs no regeneration ---<<

			ImGui::SeparatorText("Surface Detail");
			ImGui::TextWrapped(
				"Detail finer than a vertex, evaluated by the shader from the same noise the mesh was "
				"built with. Changing any of this is free -- nothing is regenerated.");

			ImGui::DragFloat("Detail Strength", &component.Detail.Strength, 0.02f, 0.0f, 5.0f);
			ImGui::DragFloat("Detail Frequency", &component.Detail.Frequency, 0.05f, 0.01f, 100.0f);
			ImGui::DragFloat("Triplanar Sharpness", &component.Detail.TriplanarSharpness, 0.1f, 1.0f, 32.0f);
			ImGui::DragFloat("Detail Fade Start", &component.Detail.FadeStart, 1.0f, 0.0f, 100000.0f);
			ImGui::DragFloat("Detail Fade End", &component.Detail.FadeEnd, 1.0f, 0.0f, 100000.0f);

			//<--- Collision ---<<

			ImGui::SeparatorText("Collision");
			ImGui::Checkbox("Generate Collision", &component.GenerateCollision);
			ImGui::SetItemTooltip("Cooks PhysX meshes for the ground near anything that could fall onto "
			                      "it, and keeps the triangles of leaf chunks in memory to do so.");

			if (component.GenerateCollision)
				ImGui::DragFloat("Collision Radius", &component.CollisionRadius, 5.0f, 1.0f, 100000.0f);

			//<--- What the generator is actually doing ---<<

			if (component.Surface)
			{
				ImGui::SeparatorText("Statistics");

				ImGui::Text("Chunks in tree: %u", component.Surface->GetChunkCount());
				ImGui::Text("Chunks drawn: %u", component.Surface->GetVisibleChunkCount());
				ImGui::Text("Building: %u", component.Surface->GetBuildingCount());
				ImGui::Text("Deepest level: %u", component.Surface->GetDeepestVisibleDepth());

				//<--- Ground the finest chunks resolve, which is the number that means anything ---<<
				const float finestChunk = component.Shape.Radius * Math::Pi * 0.5f
					/ static_cast<float>(1u << component.Lod.MaxDepth);
				const float vertexSpacing = finestChunk / static_cast<float>(
					Math::Max(1u, component.Lod.ChunkResolution));

				ImGui::Text("Finest vertex spacing: %.2f units", vertexSpacing);

				if (ImGui::Button("Regenerate"))
					component.Surface->Invalidate();
			}

			if (edited)
			{
				component.Preset = PlanetPreset::Custom;
				component.ApplySettings();
			}
		}
	};
}
