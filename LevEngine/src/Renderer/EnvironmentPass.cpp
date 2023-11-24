#include "levpch.h"
#include "EnvironmentPass.h"

#include "PipelineState.h"
#include "RenderCommand.h"
#include "RenderTarget.h"
#include "Texture.h"
#include "3D/Mesh.h"
#include "3D/Primitives.h"
#include "Assets/TextureAsset.h"
#include "DataTypes/Array.h"
#include "Scene/Components/SkyboxRenderer/SkyboxRenderer.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    EnvironmentPass::EnvironmentPass() : m_Cube(Primitives::CreateCube()) { }

    void EnvironmentPass::Process(entt::registry& registry, RenderParams& params)
    {
        const auto group = registry.group<>(entt::get<Transform, SkyboxRendererComponent>);
        for (const auto entity : group)
        {
            Transform transform = group.get<Transform>(entity);
            const SkyboxRendererComponent skybox = group.get<SkyboxRendererComponent>(entity);

            if (!skybox.SkyboxTexture) return;

            const auto& environmentMap = skybox.SkyboxTexture->GetTexture();

            if (!environmentMap) continue;

            if (environmentMap == m_EnvironmentMap) continue;

            m_EnvironmentMap = environmentMap;
            m_EnvironmentCubemap = CreateEnvironmentCubemap(environmentMap);
            m_EnvironmentIrradianceCubemap = CreateIrradianceCubemap(m_EnvironmentCubemap);
        }
    }

    Ref<Texture> EnvironmentPass::CreateEnvironmentCubemap(const Ref<Texture>& environmentMap) const
    {
        return CreateCubemap(environmentMap, 512, ShaderAssets::EquirectangularToCubemap());
    }

    Ref<Texture> EnvironmentPass::CreateIrradianceCubemap(const Ref<Texture>& environmentCubemap) const
    {
        return CreateCubemap(environmentCubemap, 512, ShaderAssets::CubemapConvolution());
    }

    Ref<Texture> EnvironmentPass::CreateCubemap(const Ref<Texture>& sourceTexture, const uint32_t resolution, const Ref<Shader>& shader) const
    {
        const Texture::TextureFormat format{Texture::Components::RGBA, Texture::Type::Float, 1, 16, 16, 16, 16};
        auto renderTexture = Texture::CreateTextureCube(resolution, resolution, format, CPUAccess::None, false, false);

        const auto renderTarget = RenderTarget::Create();
        renderTarget->AttachTexture(AttachmentPoint::Color0, renderTexture);

        const auto pipe = CreateRef<PipelineState>();
        pipe->SetShader(ShaderType::Pixel, shader);
        pipe->SetShader(ShaderType::Vertex, shader);
        pipe->SetShader(ShaderType::Geometry, shader);
        pipe->SetRenderTarget(renderTarget);
        pipe->GetRasterizerState().SetCullMode(CullMode::None);
        pipe->GetRasterizerState().SetDepthClipEnabled(false);

        const Matrix captureProjection = Matrix::CreatePerspectiveFieldOfView(Math::PiDiv2, 1.0f, 0.1f, 10.0f);
        Array<Matrix, 6> captureViews =
        {
            Matrix::CreateLookAt(Vector3::Zero, Vector3::Right, Vector3::Down) * captureProjection,
            Matrix::CreateLookAt(Vector3::Zero, Vector3::Left, Vector3::Down) * captureProjection,
            Matrix::CreateLookAt(Vector3::Zero, Vector3::Down, Vector3::Forward) * captureProjection,
            Matrix::CreateLookAt(Vector3::Zero, Vector3::Up, Vector3::Backward) * captureProjection,
            Matrix::CreateLookAt(Vector3::Zero, Vector3::Backward, Vector3::Down) * captureProjection,
            Matrix::CreateLookAt(Vector3::Zero, Vector3::Forward, Vector3::Down) * captureProjection
        };
        const auto constantBuffer = ConstantBuffer::Create(sizeof(Matrix) * 6, 6);
        constantBuffer->SetData(captureViews.data());
        pipe->GetShader(ShaderType::Geometry)->GetShaderParameterByName("cubeMapConstantBuffer").Set(constantBuffer);
        
        sourceTexture->Bind(0, ShaderType::Pixel);
        
        pipe->GetRasterizerState().SetViewport({0, 0, static_cast<float>(resolution), static_cast<float>(resolution)});
        pipe->Bind();

        m_Cube->Bind(pipe->GetShader(ShaderType::Vertex));
        RenderCommand::DrawIndexed(m_Cube->IndexBuffer);

        pipe->Unbind();

        return renderTexture;
    }
}
