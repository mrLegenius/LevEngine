#pragma once
#include "Panel.h"

namespace LevEngine::Editor
{
	enum class SceneState;

	class GamePanel final : public Panel
	{
	public:
		GamePanel(Func<SceneState> stateGetter) : Panel(), m_StateGetter(std::move(stateGetter))
		{
			m_WindowPadding = Vector2{ 0, 0 };
			m_CanScroll = false;
		}
		explicit GamePanel(const Ref<Texture>& renderTexture, Func<SceneState> stateGetter) : GamePanel(std::move(stateGetter))
		{
			m_Texture = renderTexture->Clone();

			m_Size.x = renderTexture->GetWidth();
			m_Size.y = renderTexture->GetHeight();
		}

		void UpdateTexture(const Ref<Texture>& renderTexture) const
		{
			const auto targetWidth = renderTexture->GetWidth();
			const auto targetHeight = renderTexture->GetHeight();

			if (targetWidth != m_Texture->GetWidth() || targetHeight != m_Texture->GetHeight())
				m_Texture->Resize(targetWidth, targetHeight);

			m_Texture->CopyFrom(renderTexture);
		}

		[[nodiscard]] float GetWidth() const { return m_Size.x; }
		[[nodiscard]] float GetHeight() const { return m_Size.y; }

	protected:
		String GetName() override { return "Game"; }
		void DrawContent() override;

		void OnFocus() const override;
		void OnLostFocus() const override;
	private:
		Vector2 m_Size{ 0.0f };
		Vector2 m_Bounds[2];

		Ref<Texture> m_Texture;
		Func<SceneState> m_StateGetter;
	};
}
