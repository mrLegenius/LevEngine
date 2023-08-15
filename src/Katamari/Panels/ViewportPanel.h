#pragma once
#include "PanelBase.h"
#include "Katamari/EditorCamera.h"
#include "Renderer/Texture.h"

namespace LevEngine::Editor
{
	class ViewportPanel final : public Panel
	{
	public:
		explicit ViewportPanel(const Ref<Texture>& renderTexture)
		{
			m_Texture = renderTexture->Clone();

			m_Size.x = renderTexture->GetWidth();
			m_Size.y = renderTexture->GetHeight();
		}

		[[nodiscard]] EditorCamera& GetCamera() { return m_Camera; }
		void UpdateCamera(const float deltaTime)
		{
			m_Camera.SetViewportSize(m_Size.x, m_Size.y);
			m_Camera.OnUpdate(deltaTime);
			m_Camera.UpdateView();
		}

		void UpdateViewportTexture(const Ref<Texture>& renderTexture) const
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
		std::string GetName() override { return "Viewport"; }
		void DrawContent() override;
	private:
		Vector2 m_Size{ 0.0f };
		Vector2 m_Bounds[2];

		Ref<Texture> m_Texture;

		EditorCamera m_Camera{60, 0.001, 10000, Vector3{0, 10, -10}};
	};
}

