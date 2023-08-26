#pragma once
#include "Asset.h"
#include "ObjLoader.h"
#include "Renderer/3D/Mesh.h"

namespace LevEngine
{
	class MeshAsset final : public Asset
	{
	public:
		explicit MeshAsset(const std::filesystem::path& path, const UUID uuid) : Asset(path, uuid) { }

		explicit MeshAsset(const Ref<Mesh>& mesh, const std::string& name) : Asset(name, UUID()), m_Mesh(mesh)
		{
			m_Deserialized = true;
		}

		void DrawProperties() override { }

		[[nodiscard]] const Ref<Mesh>& GetMesh() const { return m_Mesh; }
	protected:
		bool OverrideDataFile() const override { return false; }

		void SerializeData(YAML::Emitter& out) override { }
		void DeserializeData(YAML::Node& node) override
		{
			m_Mesh = ObjLoader::LoadMesh(m_Path);
		}

	private:
		Ref<Mesh> m_Mesh;
	};
}
