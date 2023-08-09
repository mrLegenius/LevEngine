#pragma once
#include "PanelBase.h"
#include "Renderer/Texture.h"

namespace LevEngine::Editor
{
	class ViewportPanel final : public Panel
	{
	public:
		explicit ViewportPanel(const Ref<Texture>& renderTexture)
		{
			m_Texture = renderTexture->Clone();
		}

		void UpdateViewportTexture(const Ref<Texture>& renderTexture) const
		{
			m_Texture->CopyFrom(renderTexture);
		}
	protected:
		std::string GetName() override { return "Viewport"; }
		void DrawContent() override;
	private:
		Vector2 m_Size{ 0.0f };
		Vector2 m_Bounds[2];

		Ref<Texture> m_Texture;
	};
}

