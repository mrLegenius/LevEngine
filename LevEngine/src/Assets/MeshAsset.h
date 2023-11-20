#pragma once
#include "Asset.h"

namespace LevEngine
{
	class Mesh;

	class MeshAsset final : public Asset
	{
	public:
		MeshAsset() = default;
		explicit MeshAsset(const Path& path, const UUID uuid) : Asset(path, uuid) { }
		
		[[nodiscard]] const Ref<Mesh>& GetMesh() const { return m_Mesh; }

		[[nodiscard]] Ref<Texture> GetIcon() const override
		{
			return Icons::Mesh();
		}
	protected:
		[[nodiscard]] bool WriteDataToFile() const override { return false; }
		[[nodiscard]] bool ReadDataFromFile() const override { return false; }

		void SerializeData(YAML::Emitter& out) override { }
		void DeserializeData(YAML::Node& node) override;

	private:
		Ref<Mesh> m_Mesh;
	};
}
