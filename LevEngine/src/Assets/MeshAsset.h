#pragma once
#include "Asset.h"
#include "ObjLoader.h"
#include "Renderer/3D/Mesh.h"
#include "DataTypes/String.h"

namespace LevEngine
{
	class MeshAsset final : public Asset
	{
	public:
		explicit MeshAsset(const Path& path, const UUID uuid) : Asset(path, uuid) { }

		void DrawProperties() override { }

		[[nodiscard]] const Ref<Mesh>& GetMesh() const { return m_Mesh; }

		[[nodiscard]] Ref<Texture> GetIcon() const override
		{
			return TextureLibrary::GetTexture("resources\\Icons\\AssetsBrowser\\MeshIcon.png");
		}
	protected:
		bool OverrideDataFile() const override { return false; }

		void SerializeData(YAML::Emitter& out) override { }
		void DeserializeData(YAML::Node& node) override
		{
			m_Mesh = ObjLoader::LoadMesh(m_Path.c_str());
		}

	private:
		Ref<Mesh> m_Mesh;
	};
}
