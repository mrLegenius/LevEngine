#pragma once
#include "RenderPass.h"

namespace LevEngine
{
	class Texture;

	class CopyTexturePass final : public RenderPass
{
public:
	CopyTexturePass(const Ref<Texture>& destinationTexture, const Ref<Texture>& sourceTexture)
	: m_DestinationTexture(destinationTexture)
	, m_SourceTexture(sourceTexture) { }

	void Process(entt::registry& registry, RenderParams& params) override;

private:
	Ref<Texture> m_DestinationTexture;
	Ref<Texture> m_SourceTexture;
};
}
