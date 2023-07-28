#pragma once

#include "Kernel/PointerUtils.h"
#include "PipelineState.h"
namespace LevEngine
{
class D3D11DeferredTechnique
{
public:
	D3D11DeferredTechnique(uint32_t width, uint32_t height);
	~D3D11DeferredTechnique();
	void BindTextures();
	void UnbindTextures();
	void StartOpaquePass();
	void StartPositionalLightingPass1();
	void StartPositionalLightingPass2();
	void StartDirectionalLightingPass();
	void EndLightningPass();

	Ref<Texture> GetDepthTexture() { return m_DepthTexture; }
	Ref<Texture> GetNormalTexture() { return m_NormalTexture; }

private:

	Ref<Texture> m_DepthTexture = nullptr;
	Ref<Texture> m_DiffuseTexture = nullptr;
	Ref<Texture> m_SpecularTexture = nullptr;
	Ref<Texture> m_NormalTexture = nullptr;

	PipelineState m_GeometryPipeline;
	PipelineState m_PositionalLightPipeline1;
	PipelineState m_PositionalLightPipeline2;
	PipelineState m_SkyboxPipeline;

	Ref<RenderTarget> m_GBufferRenderTarget;
	Ref<RenderTarget> m_DepthOnlyRenderTarget;
};
}