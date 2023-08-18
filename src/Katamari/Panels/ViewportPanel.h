#pragma once
#include "PanelBase.h"
#include "Events/KeyEvent.h"
#include "imguizmo/ImGuizmo.h"
#include "Input/Input.h"
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
			if (m_Focused)
				m_Camera.OnUpdate(deltaTime);
			m_Camera.UpdateView();
		}

		bool OnKeyPressed(KeyPressedEvent& event)
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
                m_GizmoType = -1;
                break;
            }
            case KeyCode::W:
            {
                m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
                break;
            }
            case KeyCode::E:
            {
                m_GizmoType = ImGuizmo::OPERATION::ROTATE;
                break;
            }
            case KeyCode::R:
            {
                m_GizmoType = ImGuizmo::OPERATION::SCALE;
                break;
            }

            default:
                break;
            }

			return false;
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

		EditorCamera m_Camera{30.0f, 0.1f, 1000.0f, Vector3{0, 10, -10}};

		int m_GizmoType = -1;
	};
}

