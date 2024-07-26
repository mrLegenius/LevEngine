#include "levpch.h"
#include "CopyTexturePass.h"

#include "Renderer/Pipeline/Texture.h"

namespace LevEngine
{
	String CopyTexturePass::PassName() { return m_Label; }

	void CopyTexturePass::Process(entt::registry& registry, RenderParams& params)
	{
		if (m_DestinationTexture)
			m_DestinationTexture->CopyFrom(m_SourceTexture);
	}
}

