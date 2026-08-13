#include "levpch.h"
#include "FogPass.h"

#include "Assets/EngineAssets.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/RenderParams.h"
#include "Renderer/RenderSettings.h"
#include "Renderer/Pipeline/BlendState.h"
#include "Renderer/Pipeline/ConstantBuffer.h"
#include "Renderer/Pipeline/DepthStencilState.h"
#include "Renderer/Pipeline/PipelineState.h"
#include "Renderer/Pipeline/RasterizerState.h"
#include "Renderer/Pipeline/Texture.h"
#include "Renderer/Shader/ShaderType.h"
#include "Scene/Components/Lights/Lights.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    // The depth copy goes where the G-buffer depth would be. See T_FOG_DEPTH in Registers.hlsli.
    static constexpr uint32_t k_DepthTextureSlot = 4;

    FogPass::FogPass(const Ref<RenderTarget>& renderTarget, const Ref<Texture>& depthTexture)
        : m_DepthTexture(depthTexture)
    {
        LEV_PROFILE_FUNCTION();

        m_Pipeline = CreateRef<PipelineState>();
        m_Pipeline->SetRenderTarget(renderTarget);
        m_Pipeline->SetShader(ShaderType::Vertex, ShaderAssets::Fog());
        m_Pipeline->SetShader(ShaderType::Pixel, ShaderAssets::Fog());
        m_Pipeline->GetRasterizerState().SetCullMode(CullMode::None);
        m_Pipeline->GetRasterizerState().SetDepthClipEnabled(false);
        m_Pipeline->GetDepthStencilState()->SetDepthMode(DepthMode::DisableDepthTesting);
        m_Pipeline->GetDepthStencilState()->SetStencilMode(StencilMode{false});
        m_Pipeline->GetBlendState()->SetBlendMode(BlendMode::AlphaBlending);

        m_ConstantBuffer = ConstantBuffer::Create(sizeof GPUFogData, FogConstantBufferSlot);
    }

    String FogPass::PassName() { return "Fog"; }

    bool FogPass::Begin(entt::registry& registry, RenderParams& params)
    {
        LEV_PROFILE_FUNCTION();

        if (!RenderSettings::IsFogEnabled) return false;

        //<--- The sun tints the fog when looking towards it, so the fog follows the scene's key light ---<<
        Vector3 sunDirection = Vector3::Down;
        Vector3 sunColor = Vector3::One;
        float sunScatteringIntensity = 0.0f;

        const auto directionalLights = registry.view<Transform, DirectionalLightComponent>();
        for (const auto entity : directionalLights)
        {
            auto [transform, light] = directionalLights.get<Transform, DirectionalLightComponent>(entity);

            sunDirection = transform.GetForwardDirection();
            sunColor = static_cast<Vector3>(light.color);
            sunScatteringIntensity = RenderSettings::FogSunScatteringIntensity;
            break;
        }

        const GPUFogData data{
            params.CameraPerspectiveViewProjectionMatrix.Invert(),
            params.CameraPosition,
            RenderSettings::FogDensity,
            static_cast<Vector3>(RenderSettings::FogColor),
            RenderSettings::FogHeightFalloff,
            RenderSettings::FogHeight,
            RenderSettings::FogStartDistance,
            RenderSettings::FogMaxOpacity,
            RenderSettings::IsFogAffectingSkybox ? 1.0f : 0.0f,
            sunDirection,
            sunScatteringIntensity,
            sunColor,
            RenderSettings::FogSunScatteringExponent,
        };

        m_ConstantBuffer->SetData(&data);
        m_ConstantBuffer->Bind(ShaderType::Pixel);

        m_DepthTexture->Bind(k_DepthTextureSlot, ShaderType::Pixel);

        m_Pipeline->Bind();

        return RenderPass::Begin(registry, params);
    }

    void FogPass::Process(entt::registry& registry, RenderParams& params)
    {
        LEV_PROFILE_FUNCTION();

        RenderCommand::DrawFullScreenQuad();
    }

    void FogPass::End(entt::registry& registry, RenderParams& params)
    {
        m_Pipeline->Unbind();

        m_DepthTexture->Unbind(k_DepthTextureSlot, ShaderType::Pixel);
        m_ConstantBuffer->Unbind(ShaderType::Pixel);
    }

    void FogPass::SetViewport(const Viewport viewport)
    {
        m_Pipeline->GetRasterizerState().SetViewport(viewport);
    }
}
