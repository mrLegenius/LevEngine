#pragma once
#include "Panel.h"
#include "EditorCamera.h"
#include "Gizmo.h"

namespace LevEngine::Editor
{
	class ViewportPanel final : public Panel
	{
	public:
		ViewportPanel()
		{
			m_WindowPadding = Vector2{ 0, 0 };
		}

		explicit ViewportPanel(const Ref<Texture>& renderTexture)
		{
			m_Texture = renderTexture->Clone();

			m_Size.x = renderTexture->GetWidth();
			m_Size.y = renderTexture->GetHeight();

			m_Camera.SetViewportSize(m_Size.x, m_Size.y);
		}

		[[nodiscard]] EditorCamera& GetCamera() { return m_Camera; }
		void UpdateCamera(const float deltaTime)
		{
			if (m_Focused)
				m_Camera.OnUpdate(deltaTime);
			m_Camera.UpdateView();
		}

		bool OnKeyPressed(KeyPressedEvent& event) override
		{
            //Shortcuts
            if (event.GetRepeatCount() > 0)
                return false;

			//Controlling editor camera
			if (Input::IsMouseButtonDown(MouseButton::Right)) return false;

            switch (event.GetKeyCode())
            {
            case KeyCode::Q:
            {
				Gizmo::Tool = Gizmo::ToolType::None;
                break;
            }
            case KeyCode::W:
            {
				Gizmo::Tool = Gizmo::ToolType::Translate;
                break;
            }
            case KeyCode::E:
            {
				Gizmo::Tool = Gizmo::ToolType::Rotate;
                break;
            }
            case KeyCode::R:
            {
				Gizmo::Tool = Gizmo::ToolType::Scale;
                break;
            }
            default:
                break;
            }

			return false;
		}

		void UpdateTexture(const Ref<Texture>& renderTexture)
		{
			const auto targetWidth = renderTexture->GetWidth();
			const auto targetHeight = renderTexture->GetHeight();

			if (targetWidth != m_Texture->GetWidth() || targetHeight != m_Texture->GetHeight())
			{
				m_Texture->Resize(targetWidth, targetHeight);
				m_Camera.SetViewportSize(targetWidth, targetHeight);
			}

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

		EditorCamera m_Camera{60.0f, 0.1f, 1000.0f, Vector3{0, 10, -10}};
	};
}

