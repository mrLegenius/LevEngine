#pragma once
#include "PipelineState.h"

namespace LevEngine
{
class D3D11ForwardTechnique
{
public:
	D3D11ForwardTechnique();
	~D3D11ForwardTechnique();
	void StartOpaquePass();
	void StartSkyboxPass();
	void End();

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