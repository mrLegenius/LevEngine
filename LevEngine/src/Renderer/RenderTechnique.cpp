#include "levpch.h"
#include "RenderTechnique.h"
#include "RenderParams.h"

namespace LevEngine
{
void RenderTechnique::AddPass(Ref<RenderPass> renderPass)
{
	LEV_PROFILE_FUNCTION();

	m_PassList.emplace_back(renderPass);
}

void RenderTechnique::Process(entt::registry& registry, RenderParams params) const
{
	LEV_PROFILE_FUNCTION();

	for (const auto pass : m_PassList)
		pass->Execute(registry, params);
}

void RenderTechnique::SetViewport(const Viewport& viewport)
{
	for (const auto pass : m_PassList)
		pass->SetViewport(viewport);
}
}
