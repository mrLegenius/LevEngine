#include "levpch.h"
#include "Renderer3D.h"

#include "Pipeline/ConstantBuffer.h"
#include "Pipeline/StructuredBuffer.h"
#include "RenderCommand.h"
#include "RenderSettings.h"
#include "Shader/Shader.h"
#include "3D/Mesh.h"
#include "3D/Primitives.h"
#include "Assets/MeshAsset.h"
#include "Camera/SceneCamera.h"
#include "Kernel/Application.h"
#include "Kernel/Window.h"
#include "Material/MaterialPBR.h"
#include "Scene/Components/MeshRenderer/MeshRenderer.h"

namespace LevEngine
{
    Ref<Material> Renderer3D::MissingMaterial;
    Ref<ConstantBuffer> Renderer3D::m_ModelConstantBuffer;
    Ref<ConstantBuffer> Renderer3D::m_CameraConstantBuffer;
    Ref<ConstantBuffer> Renderer3D::m_ScreenToViewParamsConstantBuffer;

    Ref<StructuredBuffer> Renderer3D::m_InstanceBuffer;
    uint32_t Renderer3D::m_InstanceBufferCapacity;

    Ref<Mesh> Renderer3D::m_CubeMesh;
    Ref<Mesh> Renderer3D::m_SphereMesh;
    Ref<Mesh> Renderer3D::m_ConeMesh;

    // The instance buffer starts here and doubles as needed, so a scene whose batch sizes settle
    // stops reallocating after the first few frames.
    static constexpr uint32_t k_InitialInstanceCapacity = 256;

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
        m_ModelConstantBuffer = ConstantBuffer::Create(sizeof MeshModelBufferData, 1);
        m_ScreenToViewParamsConstantBuffer = ConstantBuffer::Create(sizeof ScreenToViewParams, 5);

        MissingMaterial = CreateRef<MaterialPBR>();
    }

    void Renderer3D::Shutdown()
    {
        LEV_PROFILE_FUNCTION();

        m_CubeMesh.reset();
        m_SphereMesh.reset();
        m_ConeMesh.reset();

        MissingMaterial.reset();

        m_InstanceBuffer.reset();
        m_InstanceBufferCapacity = 0;

        m_ScreenToViewParamsConstantBuffer.reset();
        m_ModelConstantBuffer.reset();
        m_CameraConstantBuffer.reset();
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

        if (!mesh->IndexBuffer) return;
        
        mesh->Bind(shader);

        Matrix transposeInvertedModel;

        {
            LEV_PROFILE_SCOPE("Calculate transposed inverted model");
            transposeInvertedModel = model.Transpose().Invert();
        }

        {
            LEV_PROFILE_SCOPE("Bind Per-Instance Data");
            
            const MeshModelBufferData data = {model, transposeInvertedModel};
            m_ModelConstantBuffer->SetData(&data, sizeof(MeshModelBufferData));
            m_ModelConstantBuffer->Bind(ShaderType::Vertex);
        }

        {
            LEV_PROFILE_SCOPE("Draw indexed mesh");
            
            RenderCommand::DrawIndexed(mesh->IndexBuffer);
        }
    }

    void Renderer3D::EnsureInstanceBufferCapacity(const uint32_t count)
    {
        LEV_PROFILE_FUNCTION();

        if (m_InstanceBuffer && m_InstanceBufferCapacity >= count) return;

        uint32_t capacity = Math::Max(m_InstanceBufferCapacity, k_InitialInstanceCapacity);
        while (capacity < count)
            capacity *= 2;

        m_InstanceBuffer = StructuredBuffer::Create(nullptr, capacity, sizeof MeshInstanceData, CPUAccess::Write);
        m_InstanceBufferCapacity = capacity;
    }

    void Renderer3D::DrawMeshInstanced(const Vector<MeshInstanceData>& instances, const Ref<Mesh>& mesh,
        const Ref<Shader>& shader)
    {
        LEV_PROFILE_FUNCTION();

        if (!mesh->IndexBuffer) return;
        if (instances.empty()) return;

        const auto instanceCount = static_cast<uint32_t>(instances.size());

        EnsureInstanceBufferCapacity(instanceCount);

        {
            LEV_PROFILE_SCOPE("Bind Instance Data");

            m_InstanceBuffer->SetData(instances.data(), sizeof MeshInstanceData, 0, instanceCount);
            m_InstanceBuffer->Bind(RenderSettings::InstanceDataSlot, ShaderType::Vertex, false);
        }

        mesh->Bind(shader);

        {
            LEV_PROFILE_SCOPE("Draw indexed instanced mesh");

            RenderCommand::DrawIndexedInstanced(mesh->IndexBuffer, instanceCount);
        }
    }

    void Renderer3D::DrawMesh(const Matrix& model, const Array<Matrix, AnimationConstants::MaxBoneCount>& finalBoneMatrices, const Ref<Mesh>& mesh,
        const Ref<Shader>& shader)
    {
        LEV_PROFILE_FUNCTION();
    
        mesh->Bind(shader);

        const MeshModelBufferData data = { model, model.Transpose().Invert(), finalBoneMatrices };
        m_ModelConstantBuffer->SetData(&data, sizeof(MeshModelBufferData));
        m_ModelConstantBuffer->Bind(ShaderType::Vertex);

        RenderCommand::DrawIndexed(mesh->IndexBuffer);
    }

    void Renderer3D::DrawLineList(const Matrix& model, const Ref<Mesh>& mesh, const Ref<Shader>& shader)
    {
        LEV_PROFILE_FUNCTION();

        mesh->Bind(shader);

        const MeshModelBufferData data = {model, model.Transpose().Invert()};
        m_ModelConstantBuffer->SetData(&data, sizeof(MeshModelBufferData));
        m_ModelConstantBuffer->Bind(ShaderType::Vertex);

        RenderCommand::DrawLineList(mesh->IndexBuffer);
    }

    void Renderer3D::DrawLineStrip(const Matrix& model, const Ref<Mesh>& mesh, const Ref<Shader>& shader)
    {
        LEV_PROFILE_FUNCTION();

        mesh->Bind(shader);

        const MeshModelBufferData data = {model, model.Transpose().Invert()};
        m_ModelConstantBuffer->SetData(&data, sizeof(MeshModelBufferData));
        m_ModelConstantBuffer->Bind(ShaderType::Vertex);

        RenderCommand::DrawLineStrip(mesh->GetVerticesCount());
    }

    void Renderer3D::DrawMesh(const Matrix& model, const MeshRendererComponent& meshRenderer, const Ref<Shader>& shader)
    {
        LEV_PROFILE_FUNCTION();

        if (!meshRenderer.enabled) return;
        if (!meshRenderer.mesh) return;

        const auto mesh = meshRenderer.mesh->GetMesh();

        if (!mesh) return;

        DrawMesh(model, mesh, shader);
    }

    void Renderer3D::DrawCube(const Ref<Shader>& vertexShader)
    {
        LEV_CORE_ASSERT(vertexShader->GetType() & ShaderType::Vertex, "Cube can't be drawn without vertex shader");

        if (!m_CubeMesh) m_CubeMesh = Primitives::CreateCube();

        m_CubeMesh->Bind(vertexShader);
        RenderCommand::DrawIndexed(m_CubeMesh->IndexBuffer);
    }

    void Renderer3D::RenderSphere(const Matrix& model, const Ref<Shader>& shader)
    {
        LEV_PROFILE_FUNCTION();

        if (!m_SphereMesh) m_SphereMesh = Primitives::CreateSphere(20);

        const auto& mesh = m_SphereMesh;

        const MeshModelBufferData data = {model};
        m_ModelConstantBuffer->SetData(&data, sizeof(MeshModelBufferData));
        m_ModelConstantBuffer->Bind(ShaderType::Vertex);

        mesh->Bind(shader);

        RenderCommand::DrawIndexed(mesh->IndexBuffer);
    }

    void Renderer3D::RenderCone(const Matrix& model, const Ref<Shader>& shader)
    {
        LEV_PROFILE_FUNCTION();

        if (!m_ConeMesh) m_ConeMesh = Primitives::CreateCone(1, 1, 20);

        const auto& mesh = m_ConeMesh;

        const MeshModelBufferData data = {model};
        m_ModelConstantBuffer->SetData(&data, sizeof(MeshModelBufferData));
        m_ModelConstantBuffer->Bind(ShaderType::Vertex);

        mesh->Bind(shader);

        RenderCommand::DrawIndexed(mesh->IndexBuffer);
    }

    void Renderer3D::RenderCube(const Matrix& model, const Ref<Shader>& shader)
    {
        LEV_PROFILE_FUNCTION();

        if (!m_CubeMesh) m_CubeMesh = Primitives::CreateCube();

        const auto& mesh = m_CubeMesh;

        const MeshModelBufferData data = {model};
        m_ModelConstantBuffer->SetData(&data, sizeof(MeshModelBufferData));
        m_ModelConstantBuffer->Bind(ShaderType::Vertex);

        mesh->Bind(shader);

        RenderCommand::DrawIndexed(mesh->IndexBuffer);
    }
}
