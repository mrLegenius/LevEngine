#pragma once
#include "RenderParams.h"
namespace LevEngine
{
class RenderPass
{
public:
	virtual ~RenderPass() = default;

	void SetEnabled(const bool enabled) { m_Enabled = enabled; }
	[[nodiscard]] bool IsEnabled() const { return m_Enabled; }

	virtual bool Begin(RenderParams& params) { return true; }
	virtual void Process(RenderParams& params) = 0;
	virtual void End(RenderParams& params) { }

private:
	bool m_Enabled = true;
};
}