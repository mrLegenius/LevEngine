#include "levpch.h"
#include "RenderTechnique.h"

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
	{
		if (pass->Begin(registry, params))
		{
			pass->Process(registry, params);
			pass->End(registry, params);
		}
	}
}
}