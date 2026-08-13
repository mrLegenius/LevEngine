#include "levpch.h"
#include "EnvironmentPrecomputePass.h"

#include "EnvironmentShaders.h"
#include "Assets/TextureAsset.h"
#include "Kernel/Time/Time.h"
#include "Renderer/Pipeline/ConstantBuffer.h"
#include "Renderer/Pipeline/PipelineState.h"
#include "Renderer/Pipeline/RasterizerState.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/Pipeline/RenderTarget.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Pipeline/Texture.h"
#include "Scene/Components/SkyboxRenderer/SkyboxRenderer.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    // The sky is raymarched into this cubemap and then sampled on screen. It only has to carry a
    // smooth gradient -- the sun disks, the one sharp feature, are drawn analytically by the sky
    // shader on top.
    static constexpr uint32_t k_AtmosphereCubemapResolution = 128;
    static constexpr uint32_t k_IrradianceResolution = 32;
    static constexpr uint32_t k_PrefilterResolution = 128;

    // Refresh rate of that cubemap: fine enough that a moving sun never shows a step, and cheap
    // enough to run at it.
    static constexpr float k_SkyUpdateInterval = 0.03f;
    static const float k_SkyUpdateCosAngle = std::cos(0.25f * Math::DegToRad);

    EnvironmentPrecomputePass::EnvironmentPrecomputePass(const Ref<AtmosphereConstants>& atmosphere)
        : m_Atmosphere(atmosphere)
    {
    }

    String EnvironmentPrecomputePass::PassName() { return "Environment Precompute"; }

    void EnvironmentPrecomputePass::Process(entt::registry& registry, RenderParams& params)
    {
        if (m_Atmosphere && m_Atmosphere->IsActive())
        {
            ProcessAtmosphere();
            return;
        }

        ProcessSkybox(registry);
    }

    void EnvironmentPrecomputePass::ProcessSkybox(entt::registry& registry)
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

        // The procedural sky reuses its textures, so anything it built is stale once a skybox
        // texture takes over.
        m_HasSky = false;
        m_HasSkyLight = false;
    }

    bool EnvironmentPrecomputePass::NeedsSkyUpdate() const
    {
        if (!m_HasSky) return true;

        if (m_TimeSinceSkyUpdate < k_SkyUpdateInterval) return false;

        return m_Atmosphere->HasChangedSince(m_SkySnapshot, k_SkyUpdateCosAngle);
    }

    bool EnvironmentPrecomputePass::NeedsSkyLightUpdate() const
    {
        if (!m_HasSkyLight) return true;

        if (m_TimeSinceSkyLightUpdate < m_Atmosphere->GetSkyLightUpdateInterval()) return false;

        return m_Atmosphere->HasChangedSince(m_SkyLightSnapshot, m_Atmosphere->GetSkyLightUpdateCosAngle());
    }

    void EnvironmentPrecomputePass::ProcessAtmosphere()
    {
        LEV_PROFILE_FUNCTION();

        const float deltaTime = Time::GetUnscaledDeltaTime().GetSeconds();
        m_TimeSinceSkyUpdate += deltaTime;
        m_TimeSinceSkyLightUpdate += deltaTime;

        if (NeedsSkyUpdate())
        {
            if (!m_AtmosphereCubemap)
                m_AtmosphereCubemap = CreateRenderTexture(k_AtmosphereCubemapResolution, true);

            RenderCubemap(m_AtmosphereCubemap, k_AtmosphereCubemapResolution,
                          EnvironmentShaders::AtmosphereCubemap(), nullptr);
            m_AtmosphereCubemap->GenerateMipMaps();

            m_EnvironmentCubemap = m_AtmosphereCubemap;

            // Forget any skybox texture that was imported before, so switching back to one
            // rebuilds its cubemap instead of leaving the sky the atmosphere produced.
            m_EnvironmentMap = nullptr;

            m_SkySnapshot = m_Atmosphere->GetData();
            m_TimeSinceSkyUpdate = 0.0f;
            m_HasSky = true;
        }

        if (NeedsSkyLightUpdate())
        {
            if (!m_EnvironmentIrradianceCubemap || !m_HasSkyLight)
                m_EnvironmentIrradianceCubemap = CreateRenderTexture(k_IrradianceResolution, false);

            if (!m_EnvironmentPrefilterCubemap || !m_HasSkyLight)
                m_EnvironmentPrefilterCubemap = CreateRenderTexture(k_PrefilterResolution, true);

            RenderCubemap(m_EnvironmentIrradianceCubemap, k_IrradianceResolution,
                          EnvironmentShaders::CubemapConvolution(), m_AtmosphereCubemap);

            RenderPrefilterCubemap(m_EnvironmentPrefilterCubemap, m_AtmosphereCubemap, k_PrefilterResolution);

            if (!m_BRDFLutTexture)
                m_BRDFLutTexture = CreateBRDFLutTexture();

            m_SkyLightSnapshot = m_Atmosphere->GetData();
            m_TimeSinceSkyLightUpdate = 0.0f;
            m_HasSkyLight = true;
        }
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
        return CreateCubemap(environmentCubemap, k_IrradianceResolution,
                             EnvironmentShaders::CubemapConvolution(), false);
    }
    
    Ref<Texture> EnvironmentPrecomputePass::CreatePrefilterCubemap(const Ref<Texture>& sourceTexture)
    {
        auto renderTexture = CreateRenderTexture(k_PrefilterResolution, true);
        RenderPrefilterCubemap(renderTexture, sourceTexture, k_PrefilterResolution);

        return renderTexture;
    }

    void EnvironmentPrecomputePass::RenderPrefilterCubemap(const Ref<Texture>& destination,
                                                           const Ref<Texture>& sourceTexture,
                                                           const uint32_t resolution)
    {
        LEV_PROFILE_FUNCTION();

        const Ref<Shader> shader = EnvironmentShaders::EnvironmentPreFiltering();

        const auto renderTarget = RenderTarget::Create();
        const auto pipe = CreateCubemapPipeline(shader, renderTarget);
        
        const auto constantBuffer = ConstantBuffer::Create(sizeof(Matrix) * 6, 6);
        SetCaptureViewToShader( pipe->GetShader(ShaderType::Geometry), constantBuffer);

        const auto roughnessConstantBuffer = ConstantBuffer::Create(sizeof(float) * 4);
        
        constexpr uint8_t maxMipLevels = 7;
        for (uint8_t mip = 0; mip < maxMipLevels; ++mip)
        {
            const auto mipResolution = static_cast<uint32_t>(static_cast<float>(resolution) * std::powf(0.5f, mip));

            renderTarget->AttachTexture(AttachmentPoint::Color0, destination->GetMipMapLevel(mip));

            float roughness = static_cast<float>(mip) / static_cast<float>(maxMipLevels - 1);
            
            roughnessConstantBuffer->SetData(&roughness);
            roughnessConstantBuffer->Bind(7, ShaderType::Pixel);
            
            sourceTexture->Bind(0, ShaderType::Pixel);
            RenderCube(pipe, mipResolution);
        }
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

        const auto pipe = CreateRef<PipelineState>();
        pipe->SetShader(ShaderType::Vertex, shader);
        pipe->SetShader(ShaderType::Pixel, shader);
        pipe->SetRenderTarget(renderTarget);
        pipe->GetRasterizerState().SetCullMode(CullMode::None);
        pipe->GetRasterizerState().SetDepthClipEnabled(false);

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
        RenderCubemap(renderTexture, resolution, shader, sourceTexture);

        return renderTexture;
    }

    void EnvironmentPrecomputePass::RenderCubemap(const Ref<Texture>& destination, const uint32_t resolution,
                                                  const Ref<Shader>& shader, const Ref<Texture>& sourceTexture)
    {
        LEV_PROFILE_FUNCTION();

        const auto renderTarget = RenderTarget::Create();
        renderTarget->AttachTexture(AttachmentPoint::Color0, destination);

        const auto pipe = CreateCubemapPipeline(shader, renderTarget);

        const auto constantBuffer = ConstantBuffer::Create(sizeof(Matrix) * 6, 6);
        SetCaptureViewToShader( pipe->GetShader(ShaderType::Geometry), constantBuffer);

        // The procedural sky has no source texture: it computes its colour from the constants the
        // atmosphere pass already bound.
        if (sourceTexture)
            sourceTexture->Bind(0, ShaderType::Pixel);

        RenderCube(pipe, resolution);
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
        pipe->SetShader(ShaderType::Vertex, EnvironmentShaders::CubemapRender());
        pipe->SetShader(ShaderType::Geometry, EnvironmentShaders::CubemapRender());
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
