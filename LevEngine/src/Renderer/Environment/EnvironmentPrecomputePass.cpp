#include "levpch.h"
#include "EnvironmentPrecomputePass.h"

#include "Assets.h"
#include "EnvironmentShaders.h"
#include "Assets/TextureAsset.h"
#include "Renderer/PipelineState.h"
#include "Renderer/RasterizerState.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/Texture.h"
#include "Scene/Components/SkyboxRenderer/SkyboxRenderer.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    String EnvironmentPrecomputePass::PassName() { return "Environment Precompute"; }

    void EnvironmentPrecomputePass::Process(entt::registry& registry, RenderParams& params)
    {
        const auto group = registry.group<>(entt::get<Transform, SkyboxRendererComponent>);
        if (group.empty()) return;
        
        const auto firstEntity = group[0];
        Transform transform = group.get<Transform>(firstEntity);
        const SkyboxRendererComponent skybox = group.get<SkyboxRendererComponent>(firstEntity);
        
        if (!skybox.SkyboxTexture) return;

        const auto& environmentMap = skybox.SkyboxTexture->GetTexture();

        if (!environmentMap) return;
        if (environmentMap == m_EnvironmentMap) return;

        m_EnvironmentMap = environmentMap;
        m_EnvironmentCubemap = CreateEnvironmentCubemap(environmentMap);
        m_EnvironmentCubemap->GenerateMipMaps();
        m_EnvironmentIrradianceCubemap = CreateIrradianceCubemap(m_EnvironmentCubemap);
        m_EnvironmentPrefilterCubemap = CreatePrefilterCubemap(m_EnvironmentCubemap);
        m_BRDFLutTexture = CreateBRDFLutTexture();
    }

    void EnvironmentPrecomputePass::End(entt::registry& registry, RenderParams& params)
    {
        if (m_EnvironmentIrradianceCubemap)
            m_EnvironmentIrradianceCubemap->Bind(10, ShaderType::Pixel);

        if (m_EnvironmentPrefilterCubemap)
            m_EnvironmentPrefilterCubemap->Bind(11, ShaderType::Pixel);
        
        if (m_BRDFLutTexture)
            m_BRDFLutTexture->Bind(12, ShaderType::Pixel);
    }

    Ref<Texture> EnvironmentPrecomputePass::CreateEnvironmentCubemap(const Ref<Texture>& environmentMap)
    {
        return CreateCubemap(environmentMap, 512, EnvironmentShaders::EquirectangularToCubemap(), true);
    }

    Ref<Texture> EnvironmentPrecomputePass::CreateIrradianceCubemap(const Ref<Texture>& environmentCubemap)
    {
        return CreateCubemap(environmentCubemap, 32, EnvironmentShaders::CubemapConvolution(), false);
    }
    
    Ref<Texture> EnvironmentPrecomputePass::CreatePrefilterCubemap(const Ref<Texture>& sourceTexture)
    {
        constexpr uint32_t resolution = 128;
        const Ref<Shader> shader = EnvironmentShaders::EnvironmentPreFiltering();
        auto renderTexture = CreateRenderTexture(resolution, true);
        
        const auto renderTarget = RenderTarget::Create();
        const auto pipe = CreateCubemapPipeline(shader, renderTarget);
        
        const auto constantBuffer = ConstantBuffer::Create(sizeof(Matrix) * 6, 6);
        SetCaptureViewToShader( pipe->GetShader(ShaderType::Geometry), constantBuffer);

        const auto roughnessConstantBuffer = ConstantBuffer::Create(sizeof(float) * 4);
        
        constexpr uint8_t maxMipLevels = 7;
        for (uint8_t mip = 0; mip < maxMipLevels; ++mip)
        {
            const auto mipResolution = static_cast<uint32_t>(static_cast<float>(resolution) * std::powf(0.5f, mip));

            renderTarget->AttachTexture(AttachmentPoint::Color0, renderTexture->GetMipMapLevel(mip));

            float roughness = static_cast<float>(mip) / static_cast<float>(maxMipLevels - 1);
            
            roughnessConstantBuffer->SetData(&roughness);
            roughnessConstantBuffer->Bind(7, ShaderType::Pixel);
            
            sourceTexture->Bind(0, ShaderType::Pixel);
            RenderCube(pipe, mipResolution);
        }

        return renderTexture;
    }

    Ref<Texture> EnvironmentPrecomputePass::CreateBRDFLutTexture()
    {
        constexpr uint32_t resolution = 512;
        const auto shader = EnvironmentShaders::BRDFIntegration();
        const auto textureFormat = Texture::TextureFormat { Texture::Components::RG, Texture::Type::UnsignedNormalized,
            1,
            16, 16, 0, 0};

        const auto renderTexture = Texture::CreateTexture2D(resolution, resolution, 1, textureFormat);

        const auto renderTarget = RenderTarget::Create();
        renderTarget->AttachTexture(AttachmentPoint::Color0, renderTexture);

        const auto pipe = CreateCubemapPipeline(shader, renderTarget);

        pipe->GetRasterizerState().SetViewport({0, 0, static_cast<float>(resolution), static_cast<float>(resolution)});
        pipe->Bind();

        RenderCommand::DrawFullScreenQuad();
        
        pipe->Unbind();
        return renderTexture;
    }

    Ref<Texture> EnvironmentPrecomputePass::CreateCubemap(const Ref<Texture>& sourceTexture, const uint32_t resolution,
                                                const Ref<Shader>& shader, const bool generateMipMaps)
    {
        auto renderTexture = CreateRenderTexture(resolution, generateMipMaps);

        const auto renderTarget = RenderTarget::Create();
        renderTarget->AttachTexture(AttachmentPoint::Color0, renderTexture);

        const auto pipe = CreateCubemapPipeline(shader, renderTarget);

        const auto constantBuffer = ConstantBuffer::Create(sizeof(Matrix) * 6, 6);
        SetCaptureViewToShader( pipe->GetShader(ShaderType::Geometry), constantBuffer);
        
        sourceTexture->Bind(0, ShaderType::Pixel);

        RenderCube(pipe, resolution);

        return renderTexture;
    }

    void EnvironmentPrecomputePass::RenderCube(const Ref<PipelineState>& pipeline, const uint32_t resolution)
    {
        pipeline->GetRasterizerState().SetViewport({0, 0, static_cast<float>(resolution), static_cast<float>(resolution)});
        pipeline->Bind();

        Renderer3D::DrawCube(pipeline->GetShader(ShaderType::Vertex));

        pipeline->Unbind();
    }

    Ref<PipelineState> EnvironmentPrecomputePass::CreateCubemapPipeline(const Ref<Shader>& shader, const Ref<RenderTarget>& renderTarget)
    {
        const auto pipe = CreateRef<PipelineState>();
        pipe->SetShader(ShaderType::Pixel, shader);
        pipe->SetShader(ShaderType::Vertex, shader);
        pipe->SetShader(ShaderType::Geometry, shader);
        pipe->SetRenderTarget(renderTarget);
        pipe->GetRasterizerState().SetCullMode(CullMode::None);
        pipe->GetRasterizerState().SetDepthClipEnabled(false);
        return pipe;
    }

    Ref<Texture> EnvironmentPrecomputePass::CreateRenderTexture(const uint32_t resolution, const bool generateMipMaps)
    {
        const Texture::TextureFormat format{Texture::Components::RGBA, Texture::Type::Float, 1, 16, 16, 16, 16};
        auto renderTexture = Texture::CreateTextureCube(resolution, resolution, format, CPUAccess::None, false, generateMipMaps);
        return renderTexture;
    }

    Array<Matrix, 6> EnvironmentPrecomputePass::GetCaptureViews()
    {
        const Matrix captureProjection = Matrix::CreatePerspectiveFieldOfView(Math::PiDiv2, 1.0f, 0.1f, 10.0f);
        const Array<Matrix, 6> captureViews =
        {
            Matrix::CreateLookAt(Vector3::Zero, Vector3::Right, Vector3::Up) * captureProjection,
            Matrix::CreateLookAt(Vector3::Zero, Vector3::Left, Vector3::Up) * captureProjection,
            Matrix::CreateLookAt(Vector3::Zero, Vector3::Up, Vector3::Backward) * captureProjection,
            Matrix::CreateLookAt(Vector3::Zero, Vector3::Down, Vector3::Forward) * captureProjection,
            Matrix::CreateLookAt(Vector3::Zero, Vector3::Forward, Vector3::Up) * captureProjection,
            Matrix::CreateLookAt(Vector3::Zero, Vector3::Backward, Vector3::Up) * captureProjection,
        };

        return captureViews;
    }

    void EnvironmentPrecomputePass::SetCaptureViewToShader(const Ref<Shader>& shader, const Ref<ConstantBuffer>& constantBuffer)
    {
        Array<Matrix, 6> captureViews = GetCaptureViews();
        constantBuffer->SetData(captureViews.data());
        shader->GetShaderParameterByName("cubeMapConstantBuffer").Set(constantBuffer);
    }
}
