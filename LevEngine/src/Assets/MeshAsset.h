#pragma once
#include "Asset.h"
#include "SkeletonAsset.h"
#include "Renderer/3D/Mesh.h"

namespace LevEngine
{
	class Mesh;
	class Skeleton;

	class MeshAsset final : public Asset
	{
	public:
		MeshAsset() = default;
		explicit MeshAsset(const Path& path, const UUID uuid) : Asset(path, uuid) { }
		
		[[nodiscard]] const Ref<Mesh>& GetMesh() const { return m_Mesh; }

		[[nodiscard]] Ref<Texture> GetIcon() const override;

	protected:
		[[nodiscard]] bool WriteDataToFile() const override { return false; }
		[[nodiscard]] bool ReadDataFromFile() const override { return false; }

		[[nodiscard]] bool WriteLibraryToFile() const override { return true; }
		[[nodiscard]] bool ReadLibraryFromFile() const override { return true; }

		void SerializeData(YAML::Emitter& out) override;
		void DeserializeData(const YAML::Node& node) override;

		void SerializeToBinaryLibrary() override;
		void DeserializeFromBinaryLibrary() override;

	private:
		const char* c_VertexListKey = "VertexList";
		const char* c_IndexListKey = "IndexList";
		const char* c_UVListKey = "UVList";
		const char* c_NormalListKey = "NormalList";
		const char* c_TangentListKey = "TangentList";
		const char* c_BiTangentListKey = "BiTangentList";
		const char* c_BoneIDListKey = "BoneIDList";
		const char* c_BoneWeightListKey = "BoneWeightList";
		const char* c_AABBCenterKey = "AABBCenter";
		const char* c_AABBExtentsKey = "AABBExtents";

		void ParseMeshFromUniversalFormat();
		void CreateAnimationAsset(const Vector<Ref<Animation>>& animations, const Ref<SkeletonAsset>& skeletonAsset) const;
		Ref<SkeletonAsset> CreateSkeletonAsset(const Ref<Skeleton>& resultSkeleton);

		Ref<Mesh> m_Mesh;
	};
}
