#include "levpch.h"
#include "EnvironmentPass.h"

#include "PipelineState.h"
#include "RasterizerState.h"
#include "RenderCommand.h"
#include "Renderer3D.h"
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
    static auto EnvironmentPreFiltering()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("Environment/PreFiltering.hlsl"),
            ShaderType::Pixel | ShaderType::Vertex | ShaderType::Geometry);
        return shader;
    }

    static auto BRDFIntegration()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("Environment/BRDFIntegration.hlsl"),
            ShaderType::Pixel | ShaderType::Vertex);
        return shader;
    }
    
    EnvironmentPass::EnvironmentPass() : m_Cube(Primitives::CreateCube()) { }

    String EnvironmentPass::PassName() { return "Environment Precompute"; }

    void EnvironmentPass::Process(entt::registry& registry, RenderParams& params)
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

    void EnvironmentPass::End(entt::registry& registry, RenderParams& params)
    {
        if (m_EnvironmentIrradianceCubemap)
            m_EnvironmentIrradianceCubemap->Bind(10, ShaderType::Pixel);

        if (m_EnvironmentPrefilterCubemap)
            m_EnvironmentPrefilterCubemap->Bind(11, ShaderType::Pixel);
        
        if (m_BRDFLutTexture)
            m_BRDFLutTexture->Bind(12, ShaderType::Pixel);
    }

    Ref<Texture> EnvironmentPass::CreateEnvironmentCubemap(const Ref<Texture>& environmentMap) const
    {
        return CreateCubemap(environmentMap, 512, ShaderAssets::EquirectangularToCubemap(), true);
    }

    Ref<Texture> EnvironmentPass::CreateIrradianceCubemap(const Ref<Texture>& environmentCubemap) const
    {
        return CreateCubemap(environmentCubemap, 32, ShaderAssets::CubemapConvolution(), false);
    }
    
    Ref<Texture> EnvironmentPass::CreatePrefilterCubemap(const Ref<Texture>& sourceTexture) const
    {
        constexpr uint32_t resolution = 128;
        const Ref<Shader> shader = EnvironmentPreFiltering();
        auto renderTexture = CreateRenderTexture(resolution, true);
        
        const auto renderTarget = RenderTarget::Create();

        const auto pipe = CreateRef<PipelineState>();
        pipe->SetShader(ShaderType::Pixel, shader);
        pipe->SetShader(ShaderType::Vertex, shader);
        pipe->SetShader(ShaderType::Geometry, shader);
        pipe->SetRenderTarget(renderTarget);
        pipe->GetRasterizerState().SetCullMode(CullMode::None);
        pipe->GetRasterizerState().SetDepthClipEnabled(false);
        
        const auto constantBuffer = ConstantBuffer::Create(sizeof(Matrix) * 6, 6);
        SetCaptureViewToShader( pipe->GetShader(ShaderType::Geometry), constantBuffer);

        const auto roughnessConstantBuffer = ConstantBuffer::Create(sizeof(float) * 4);
        
        constexpr uint8_t maxMipLevels = 7;
        for (uint8_t mip = 0; mip < maxMipLevels; ++mip)
        {
            const unsigned int mipWidth  = static_cast<float>(resolution) * std::powf(0.5f, mip);
            const unsigned int mipHeight = static_cast<float>(resolution) * std::powf(0.5f, mip);
            
            renderTarget->AttachTexture(AttachmentPoint::Color0, renderTexture->GetMipMapLevel(mip));

            float roughness = static_cast<float>(mip) / static_cast<float>(maxMipLevels - 1);
            
            roughnessConstantBuffer->SetData(&roughness);
            roughnessConstantBuffer->Bind(7, ShaderType::Pixel);
            
            sourceTexture->Bind(0, ShaderType::Pixel);
            pipe->GetRasterizerState().SetViewport({0, 0, static_cast<float>(mipWidth), static_cast<float>(mipHeight)});
            pipe->Bind();
    
            m_Cube->Bind(pipe->GetShader(ShaderType::Vertex));
            RenderCommand::DrawIndexed(m_Cube->IndexBuffer);
        }
        
        pipe->Unbind();

        return renderTexture;
    }

    Ref<Texture> EnvironmentPass::CreateBRDFLutTexture() const
    {
        constexpr uint32_t resolution = 512;
        const auto shader = BRDFIntegration();
        const auto textureFormat = Texture::TextureFormat { Texture::Components::RG, Texture::Type::UnsignedNormalized,
            1,
            16, 16, 0, 0};

        const auto renderTexture = Texture::CreateTexture2D(resolution, resolution, 1, textureFormat);

        const auto renderTarget = RenderTarget::Create();
        renderTarget->AttachTexture(AttachmentPoint::Color0, renderTexture);

        const auto pipe = CreateRef<PipelineState>();
        pipe->SetShader(ShaderType::Pixel, shader);
        pipe->SetShader(ShaderType::Vertex, shader);
        pipe->SetRenderTarget(renderTarget);
        pipe->GetRasterizerState().SetCullMode(CullMode::None);
        pipe->GetRasterizerState().SetDepthClipEnabled(false);

        pipe->GetRasterizerState().SetViewport({0, 0, static_cast<float>(resolution), static_cast<float>(resolution)});
        pipe->Bind();

        RenderCommand::DrawFullScreenQuad();
        
        pipe->Unbind();
        return renderTexture;
    }

    Ref<Texture> EnvironmentPass::CreateCubemap(const Ref<Texture>& sourceTexture, const uint32_t resolution,
                                                const Ref<Shader>& shader, const bool generateMipMaps) const
    {
        auto renderTexture = CreateRenderTexture(resolution, generateMipMaps);

        const auto renderTarget = RenderTarget::Create();
        renderTarget->AttachTexture(AttachmentPoint::Color0, renderTexture);

        const auto pipe = CreateRef<PipelineState>();
        pipe->SetShader(ShaderType::Pixel, shader);
        pipe->SetShader(ShaderType::Vertex, shader);
        pipe->SetShader(ShaderType::Geometry, shader);
        pipe->SetRenderTarget(renderTarget);
        pipe->GetRasterizerState().SetCullMode(CullMode::None);
        pipe->GetRasterizerState().SetDepthClipEnabled(false);

        const auto constantBuffer = ConstantBuffer::Create(sizeof(Matrix) * 6, 6);
        SetCaptureViewToShader( pipe->GetShader(ShaderType::Geometry), constantBuffer);
        
        sourceTexture->Bind(0, ShaderType::Pixel);

        pipe->GetRasterizerState().SetViewport({0, 0, static_cast<float>(resolution), static_cast<float>(resolution)});
        pipe->Bind();

        m_Cube->Bind(pipe->GetShader(ShaderType::Vertex));
        RenderCommand::DrawIndexed(m_Cube->IndexBuffer);

        pipe->Unbind();

        return renderTexture;
    }

    Ref<Texture> EnvironmentPass::CreateRenderTexture(const uint32_t resolution, const bool generateMipMaps)
    {
        const Texture::TextureFormat format{Texture::Components::RGBA, Texture::Type::Float, 1, 16, 16, 16, 16};
        auto renderTexture = Texture::CreateTextureCube(resolution, resolution, format, CPUAccess::None, false, generateMipMaps);
        return renderTexture;
    }

    Array<Matrix, 6> EnvironmentPass::GetCaptureViews()
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

    void EnvironmentPass::SetCaptureViewToShader(const Ref<Shader>& shader, const Ref<ConstantBuffer>& constantBuffer)
    {
        Array<Matrix, 6> captureViews = GetCaptureViews();
        constantBuffer->SetData(captureViews.data());
        shader->GetShaderParameterByName("cubeMapConstantBuffer").Set(constantBuffer);
    }
}
