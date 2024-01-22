#pragma once
#include "RenderSettings.h"
#include "Math/Math.h"
#include "3D/AnimationConstants.h"
#include "DataTypes/Array.h"

namespace LevEngine
{
    class Shader;
    class ConstantBuffer;
    struct MeshRendererComponent;
    class SceneCamera;
    class Mesh;

    struct StaticMeshModelBufferData
    {
        Matrix Model;
        Matrix TransposedInvertedModel;
    };

    struct AnimatedMeshModelBufferData
    {
        Matrix Model;
        Matrix TransposedInvertedModel;
    	Array<Matrix, AnimationConstants::MaxBoneCount> FinalBoneMatrices;
    };

    class Renderer3D
    {
    public:
        static void Init();

        static void SetCameraBuffer(const SceneCamera* camera, const Matrix& viewMatrix, const Vector3& position);
        static void DrawMesh(const Matrix& model, const Ref<Mesh>& mesh, const Ref<Shader>& shader);
        static void DrawMesh(const Matrix& model, const MeshRendererComponent& meshRenderer, const Ref<Shader>& shader);
    	static void DrawMesh(const Matrix& model, const Array<Matrix, AnimationConstants::MaxBoneCount>& finalBoneMatrices,
			const Ref<Mesh>& mesh, const Ref<Shader>& shader);
        static void DrawMesh(const Matrix& model, const Array<Matrix, AnimationConstants::MaxBoneCount>& finalBoneMatrices,
            const MeshRendererComponent& meshRenderer, const Ref<Shader>& shader);
        static void DrawCube(const Ref<Shader>& vertexShader);

        static void DrawLineList(const Matrix& model, const Ref<Mesh>& mesh, const Ref<Shader>& shader);
        static void DrawLineStrip(const Matrix& model, const Ref<Mesh>& mesh, const Ref<Shader>& shader);
        
        static void RenderSphere(const Matrix& model, const Ref<Shader>& shader);
        static void RenderCone(const Matrix& model, const Ref<Shader>& shader);
        static void RenderCube(const Matrix& model, const Ref<Shader>& shader);

    private:
        static Ref<ConstantBuffer> m_StaticModelConstantBuffer;
        static Ref<ConstantBuffer> m_AnimatedModelConstantBuffer;
        static Ref<ConstantBuffer> m_CameraConstantBuffer;
        static Ref<ConstantBuffer> m_ScreenToViewParamsConstantBuffer;

        static Matrix m_ViewProjection;
    };
}
