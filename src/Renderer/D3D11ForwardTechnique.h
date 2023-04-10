#pragma once

#include <d3d11.h>

#include "Kernel/PointerUtils.h"
#include "PipelineState.h"

class D3D11ForwardTechnique
{
public:
	D3D11ForwardTechnique();
	~D3D11ForwardTechnique();
	void StartOpaquePass();
	void StartSkyboxPass();
	void End();

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
