#pragma once

#include "Kernel/Core.h"
#include "Asset.h"
#include "Planet/PlanetBiome.h"

namespace LevEngine
{
	class Texture;
	class TextureAsset;

	// The biomes of a planet and the textures the ground is drawn with, as one file so that a planet
	// can be pointed at a different set without touching anything else.
	//
	// The textures are a separate list from the biomes, indexed by PlanetBiome::TextureIndex, because
	// several biomes share ground: savanna and steppe are the same dry grass at different
	// temperatures, and a set with one entry for each is a set with two identical slices in every
	// array. Keeping them apart also means the arrays stay small, and array size is what the shader
	// pays for.
	class LEV_API PlanetBiomeSetAsset final : public Asset
	{
	public:
		// One entry in the texture arrays. Any of the three may be missing: a null albedo leaves the
		// biome to its tint, and a null normal or roughness leaves the surface flat and matte, both of
		// which are legitimate for a planet that has not been dressed yet.
		struct GroundTextures
		{
			String Name = "Ground";

			Ref<TextureAsset> Albedo;
			Ref<TextureAsset> Normal;
			Ref<TextureAsset> Roughness;
		};

		explicit PlanetBiomeSetAsset(const Path& path, const UUID uuid) : Asset(path, uuid) { }

		[[nodiscard]] const Vector<PlanetBiome>& GetBiomes() const { return m_Biomes; }
		[[nodiscard]] Vector<PlanetBiome>& GetBiomes() { return m_Biomes; }

		[[nodiscard]] const Vector<GroundTextures>& GetTextures() const { return m_Textures; }
		[[nodiscard]] Vector<GroundTextures>& GetTextures() { return m_Textures; }

		// Built on first use rather than on load, because loading happens while the asset database is
		// walking the project and the render device may have nothing to say about a texture yet.
		// Null when the set has no textures at all, which the shader takes as "use the tints".
		[[nodiscard]] const Ref<Texture>& GetAlbedoArray();
		[[nodiscard]] const Ref<Texture>& GetNormalArray();
		[[nodiscard]] const Ref<Texture>& GetRoughnessArray();

		//<--- Drops the arrays, so the next frame rebuilds them from whatever the textures are now ---<<
		void InvalidateTextures();

		//<--- Fills the set with the default Earthlike biomes and no textures ---<<
		void ResetToEarthlike();

		[[nodiscard]] Ref<Texture> GetIcon() const override;

	protected:
		void SerializeData(YAML::Emitter& out) override;
		void DeserializeData(const YAML::Node& node) override;

	private:
		enum class Channel
		{
			Albedo,
			Normal,
			Roughness,
		};

		void BuildArrays();
		[[nodiscard]] Ref<Texture> BuildArray(Channel channel) const;

		Vector<PlanetBiome> m_Biomes;
		Vector<GroundTextures> m_Textures;

		Ref<Texture> m_AlbedoArray;
		Ref<Texture> m_NormalArray;
		Ref<Texture> m_RoughnessArray;

		bool m_ArraysBuilt = false;
	};
}
