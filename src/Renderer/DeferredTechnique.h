#pragma once
#include "PipelineState.h"

namespace LevEngine
{
class DeferredTechnique
{
public:
	DeferredTechnique(uint32_t width, uint32_t height);
	~DeferredTechnique();
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

	Ref<RenderTarget> m_GBufferRenderTarget;
	Ref<RenderTarget> m_DepthOnlyRenderTarget;
};
}