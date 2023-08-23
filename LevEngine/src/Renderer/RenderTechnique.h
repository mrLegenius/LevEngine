#pragma once
#include "RenderPass.h"

namespace LevEngine
{
class RenderTechnique
{
public:
	RenderTechnique() = default;

	void AddPass(Ref<RenderPass> renderPass)
	{
		LEV_PROFILE_FUNCTION();

		m_PassList.emplace_back(renderPass);
	}

	void Process(entt::registry& registry, RenderParams params) const
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

private:
	using PassList = std::vector<Ref<RenderPass>>;

	PassList m_PassList;
};
}
