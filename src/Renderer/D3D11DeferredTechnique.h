#pragma once
#include <d3d11.h>

#include "Kernel/PointerUtils.h"
#include "PipelineState.h"

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
	void StartSkyboxPass();
	void EndLightningPass();

private:

	Ref<D3D11Texture> m_DepthTexture = nullptr;
	Ref<D3D11Texture> m_DiffuseTexture = nullptr;
	Ref<D3D11Texture> m_SpecularTexture = nullptr;
	Ref<D3D11Texture> m_NormalTexture = nullptr;

	PipelineState m_GeometryPipeline;
	PipelineState m_PositionalLightPipeline1;
	PipelineState m_PositionalLightPipeline2;
	PipelineState m_SkyboxPipeline;

	Ref<D3D11RenderTarget> m_GBufferRenderTarget;
	Ref<D3D11RenderTarget> m_DepthOnlyRenderTarget;
};
