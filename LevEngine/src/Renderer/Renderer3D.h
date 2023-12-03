#pragma once
#include "RenderSettings.h"
#include "Math/Math.h"

namespace LevEngine
{
    class Shader;
    class ConstantBuffer;
    struct MeshRendererComponent;
    class SceneCamera;
    class Mesh;

    struct MeshModelBufferData
    {
        Matrix Model;
        Matrix TransposedInvertedModel;
    };

    class Renderer3D
    {
    public:
        static void Init();

        static void SetCameraBuffer(const SceneCamera* camera, const Matrix& viewMatrix, const Vector3& position);
        static void DrawMesh(const Matrix& model, const Ref<Mesh>& mesh, const Ref<Shader>& shader);
        static void DrawMesh(const Matrix& model, const MeshRendererComponent& meshRenderer, const Ref<Shader>& shader);
        static void DrawCube(const Ref<Shader>& vertexShader);

        static void DrawLineList(const Matrix& model, const Ref<Mesh>& mesh, const Ref<Shader>& shader);
        static void DrawLineStrip(const Matrix& model, const Ref<Mesh>& mesh, const Ref<Shader>& shader);
        
        static void RenderSphere(const Matrix& model, const Ref<Shader>& shader);
        static void RenderCone(const Matrix& model, const Ref<Shader>& shader);
        static void RenderCube(const Matrix& model, const Ref<Shader>& shader);

    private:
        static Ref<ConstantBuffer> m_ModelConstantBuffer;
        static Ref<ConstantBuffer> m_CameraConstantBuffer;
        static Ref<ConstantBuffer> m_ScreenToViewParamsConstantBuffer;

        static Matrix m_ViewProjection;
    };
}
