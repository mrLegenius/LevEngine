#include "levpch.h"
#include "Renderer3D.h"

#include "ConstantBuffer.h"
#include "RenderCommand.h"
#include "Shader.h"
#include "3D/Mesh.h"
#include "3D/Primitives.h"
#include "Assets/MeshAsset.h"
#include "Camera/SceneCamera.h"
#include "Kernel/Application.h"
#include "Kernel/Window.h"
#include "Scene/Components/MeshRenderer/MeshRenderer.h"

namespace LevEngine
{
    Ref<ConstantBuffer> Renderer3D::m_StaticModelConstantBuffer;
    Ref<ConstantBuffer> Renderer3D::m_AnimatedModelConstantBuffer;
    Ref<ConstantBuffer> Renderer3D::m_CameraConstantBuffer;
    Ref<ConstantBuffer> Renderer3D::m_ScreenToViewParamsConstantBuffer;

    Matrix Renderer3D::m_ViewProjection;

    struct alignas(16) CameraData
    {
        Matrix View;
        Matrix ViewProjection;
        Vector3 Position;
    };

    struct alignas(16) ScreenToViewParams
    {
        Matrix CameraInvertedProjection;
        Vector2 ScreenDimensions;
    };

    void Renderer3D::Init()
    {
        LEV_PROFILE_FUNCTION();

        m_CameraConstantBuffer = ConstantBuffer::Create(sizeof CameraData, 0);
        m_StaticModelConstantBuffer = ConstantBuffer::Create(sizeof StaticMeshModelBufferData, 1);
        m_AnimatedModelConstantBuffer = ConstantBuffer::Create(sizeof AnimatedMeshModelBufferData, 1);
        m_ScreenToViewParamsConstantBuffer = ConstantBuffer::Create(sizeof ScreenToViewParams, 5);
    }

    void Renderer3D::SetCameraBuffer(const SceneCamera* camera, const Matrix& viewMatrix, const Vector3& position)
    {
        LEV_PROFILE_FUNCTION();

        const auto& window = Application::Get().GetWindow();
        const float width = static_cast<float>(window.GetWidth());
        const float height = static_cast<float>(window.GetHeight());

        const auto viewProjection = viewMatrix * camera->GetProjection();

        const CameraData cameraData{viewMatrix, viewProjection, position};
        m_CameraConstantBuffer->SetData(&cameraData);
        m_CameraConstantBuffer->Bind(ShaderType::Vertex | ShaderType::Pixel);

        const ScreenToViewParams params{camera->GetProjection().Invert(), Vector2{width, height}};
        m_ScreenToViewParamsConstantBuffer->SetData(&params);
        m_ScreenToViewParamsConstantBuffer->Bind(ShaderType::Pixel);
    }

    void Renderer3D::DrawMesh(const Matrix& model, const Ref<Mesh>& mesh, const Ref<Shader>& shader)
    {
        LEV_PROFILE_FUNCTION();

        mesh->Bind(shader);

        const StaticMeshModelBufferData data = {model, model.Transpose().Invert()};
        m_StaticModelConstantBuffer->SetData(&data, sizeof(StaticMeshModelBufferData));
        m_StaticModelConstantBuffer->Bind(ShaderType::Vertex);

        RenderCommand::DrawIndexed(mesh->IndexBuffer);
    }

    void Renderer3D::DrawMesh(const Matrix& model,
        const Array<Matrix, AnimationConstants::MaxBoneCount>& finalBoneMatrices, const Ref<Mesh>& mesh,
        const Ref<Shader>& shader)
    {
        LEV_PROFILE_FUNCTION();
    
        mesh->Bind(shader);

        const AnimatedMeshModelBufferData data = { model, model.Transpose().Invert(), finalBoneMatrices };
        m_AnimatedModelConstantBuffer->SetData(&data, sizeof(AnimatedMeshModelBufferData));
        m_AnimatedModelConstantBuffer->Bind(ShaderType::Vertex);

        RenderCommand::DrawIndexed(mesh->IndexBuffer);
    }

    void Renderer3D::DrawMesh(const Matrix& model,
        const Array<Matrix, AnimationConstants::MaxBoneCount>& finalBoneMatrices,
        const MeshRendererComponent& meshRenderer, const Ref<Shader>& shader)
    {
        LEV_PROFILE_FUNCTION();

        if (!meshRenderer.mesh) return;

        const auto mesh = meshRenderer.mesh->GetMesh();

        if (!mesh) return;

        DrawMesh(model, finalBoneMatrices, mesh, shader);
    }

    void Renderer3D::DrawLineList(const Matrix& model, const Ref<Mesh>& mesh, const Ref<Shader>& shader)
    {
        LEV_PROFILE_FUNCTION();

        mesh->Bind(shader);

        const StaticMeshModelBufferData data = {model, model.Transpose().Invert()};
        m_StaticModelConstantBuffer->SetData(&data, sizeof(StaticMeshModelBufferData));
        m_StaticModelConstantBuffer->Bind(ShaderType::Vertex);

        RenderCommand::DrawLineList(mesh->IndexBuffer);
    }

    void Renderer3D::DrawLineStrip(const Matrix& model, const Ref<Mesh>& mesh, const Ref<Shader>& shader)
    {
        LEV_PROFILE_FUNCTION();

        mesh->Bind(shader);

        const StaticMeshModelBufferData data = {model, model.Transpose().Invert()};
        m_StaticModelConstantBuffer->SetData(&data, sizeof(StaticMeshModelBufferData));
        m_StaticModelConstantBuffer->Bind(ShaderType::Vertex);

        RenderCommand::DrawLineStrip(mesh->GetVerticesCount());
    }

    void Renderer3D::DrawMesh(const Matrix& model, const MeshRendererComponent& meshRenderer, const Ref<Shader>& shader)
    {
        LEV_PROFILE_FUNCTION();

        if (!meshRenderer.mesh) return;

        const auto mesh = meshRenderer.mesh->GetMesh();

        if (!mesh) return;

        DrawMesh(model, mesh, shader);
    }

    void Renderer3D::DrawCube(const Ref<Shader>& vertexShader)
    {
        LEV_CORE_ASSERT(vertexShader->GetType() & ShaderType::Vertex, "Cube can't be drawn without vertex shader");

        static Ref<Mesh> cube = Primitives::CreateCube();
        cube->Bind(vertexShader);
        RenderCommand::DrawIndexed(cube->IndexBuffer);
    }

    void Renderer3D::RenderSphere(const Matrix& model, const Ref<Shader>& shader)
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Mesh> mesh = Primitives::CreateSphere(20);

        const StaticMeshModelBufferData data = {model};
        m_StaticModelConstantBuffer->SetData(&data, sizeof(StaticMeshModelBufferData));
        m_StaticModelConstantBuffer->Bind(ShaderType::Vertex);

        mesh->Bind(shader);

        RenderCommand::DrawIndexed(mesh->IndexBuffer);
    }

    void Renderer3D::RenderCone(const Matrix& model, const Ref<Shader>& shader)
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Mesh> mesh = Primitives::CreateCone(1, 1, 20);

        const StaticMeshModelBufferData data = {model};
        m_StaticModelConstantBuffer->SetData(&data, sizeof(StaticMeshModelBufferData));
        m_StaticModelConstantBuffer->Bind(ShaderType::Vertex);

        mesh->Bind(shader);

        RenderCommand::DrawIndexed(mesh->IndexBuffer);
    }

    void Renderer3D::RenderCube(const Matrix& model, const Ref<Shader>& shader)
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Mesh> mesh = Primitives::CreateCube();

        const StaticMeshModelBufferData data = {model};
        m_StaticModelConstantBuffer->SetData(&data, sizeof(StaticMeshModelBufferData));
        m_StaticModelConstantBuffer->Bind(ShaderType::Vertex);

        mesh->Bind(shader);

        RenderCommand::DrawIndexed(mesh->IndexBuffer);
    }
}
