#include "levpch.h"
#include "CopyTexturePass.h"

namespace LevEngine
{
	void CopyTexturePass::Process(entt::registry& registry, RenderParams& params)
	{
		if (m_DestinationTexture)
			m_DestinationTexture->CopyFrom(m_SourceTexture);
	}
}

