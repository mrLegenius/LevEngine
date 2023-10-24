#pragma once
#include "entt/entt.hpp"

#include "RenderParams.h"
#include "Viewport.h"

namespace LevEngine
{
class RenderTechnique;

class RenderPass
{
public:
	virtual ~RenderPass() = default;

	void SetEnabled(const bool enabled) { m_Enabled = enabled; }
	[[nodiscard]] bool IsEnabled() const { return m_Enabled; }
	
	friend RenderTechnique;

	bool Execute(entt::registry& registry, RenderParams& params)
	{
		if (Begin(registry, params))
		{
			Process(registry, params);
			End(registry, params);

			return true;
		}

		return false;
	}

	virtual void SetViewport(Viewport viewport) { }
	
protected:
	
	virtual bool Begin(entt::registry& registry, RenderParams& params) { return true; }
	virtual void Process(entt::registry& registry, RenderParams& params) = 0;
	virtual void End(entt::registry& registry, RenderParams& params) { }

private:
	bool m_Enabled = true;
};
}
