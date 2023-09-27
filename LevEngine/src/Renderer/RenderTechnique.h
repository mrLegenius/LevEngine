#pragma once
#include "RenderPass.h"

namespace LevEngine
{
class RenderTechnique
{
public:
	RenderTechnique() = default;

	void AddPass(Ref<RenderPass> renderPass);
	void Process(entt::registry& registry, RenderParams params) const;

private:
	using PassList = Vector<Ref<RenderPass>>;

	PassList m_PassList;
};
}
