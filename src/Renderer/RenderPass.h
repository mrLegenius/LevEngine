#pragma once
#include "entt/entt.hpp"

#include "RenderParams.h"
namespace LevEngine
{
class RenderTechnique;

class RenderPass
{
public:
	virtual ~RenderPass() = default;

	void SetEnabled(const bool enabled) { m_Enabled = enabled; }
	[[nodiscard]] bool IsEnabled() const { return m_Enabled; }

protected:
	friend RenderTechnique;

	virtual bool Begin(entt::registry& registry, RenderParams& params) { return true; }
	virtual void Process(entt::registry& registry, RenderParams& params) = 0;
	virtual void End(entt::registry& registry, RenderParams& params) { }

private:
	bool m_Enabled = true;
};
}