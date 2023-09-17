#pragma once
#include "Asset.h"
#include "MeshLoader.h"
#include "Renderer/3D/Mesh.h"

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
			return Icons::Mesh();
		}
	protected:
		bool OverrideDataFile() const override { return false; }

		void SerializeData(YAML::Emitter& out) override { }
		void DeserializeData(YAML::Node& node) override
		{
			try
			{
				m_Mesh = MeshLoader::LoadMesh(m_Path);
			}
			catch (std::exception& e)
			{
				Log::CoreWarning("Failed to load mesh in {0}. Error: {1}", m_Path.string(), e.what());
			}
		}

	private:
		Ref<Mesh> m_Mesh;
	};
}
