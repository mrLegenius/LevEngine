#pragma once
#include "Asset.h"
#include "ObjLoader.h"
#include "Renderer/3D/Mesh.h"

namespace LevEngine
{
	class MeshAsset final : public Asset
	{
	public:
		explicit MeshAsset(const std::filesystem::path& path, const UUID uuid) : Asset(path, uuid)
		{
			m_Mesh = ObjLoader::LoadMesh(path);
		}
		explicit MeshAsset(const Ref<Mesh>& mesh, const std::string& name) : Asset(name, UUID()), m_Mesh(mesh)
		{

		}

		void DrawProperties() override { }

		[[nodiscard]] const Ref<Mesh>& GetMesh() const { return m_Mesh; }
	protected:
		void SerializeData(YAML::Emitter& out) override { }
		bool DeserializeData(YAML::Node& node) override { return true; }

	private:
		Ref<Mesh> m_Mesh;
	};
}
