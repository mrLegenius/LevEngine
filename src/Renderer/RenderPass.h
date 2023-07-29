#pragma once
#include "entt/entt.hpp"

#include "RenderParams.h"
namespace LevEngine
{
class RenderPass
{
public:
	virtual ~RenderPass() = default;

	void SetEnabled(const bool enabled) { m_Enabled = enabled; }
	[[nodiscard]] bool IsEnabled() const { return m_Enabled; }

	void Execute(entt::registry& registry, RenderParams& params)
	{
		const auto success = Begin(registry, params);
		if (success)
		{
			Process(registry, params);
			End(registry, params);
		}
	}

protected:
	virtual bool Begin(entt::registry& registry, RenderParams& params) { return true; }
	virtual void Process(entt::registry& registry, RenderParams& params) = 0;
	virtual void End(entt::registry& registry, RenderParams& params) { }

private:
	bool m_Enabled = true;
};
}