#pragma once
#include "Panel.h"
#include "EditorCamera.h"
#include "Gizmo.h"

namespace LevEngine::Editor
{
	class ViewportPanel final : public Panel
	{
	public:
		ViewportPanel();

		explicit ViewportPanel(const Ref<Texture>& renderTexture);

		[[nodiscard]] EditorCamera& GetCamera() { return m_Camera; }
		void UpdateCamera(const float deltaTime);

		bool OnKeyPressed(KeyPressedEvent& event) override;

		void UpdateTexture(const Ref<Texture>& renderTexture);

		[[nodiscard]] float GetWidth() const { return m_Size.x; }
		[[nodiscard]] float GetHeight() const { return m_Size.y; }

	protected:
		String GetName() override { return "Viewport"; }
		void DrawContent() override;
		void DrawGizmo() const;

	private:

		void DrawToolbar();

		Vector2 m_Size{ 0.0f };
		Vector2 m_Bounds[2];

		Ref<Texture> m_Texture;

		EditorCamera m_Camera{60.0f, 0.1f, 1000.0f, Vector3{0, 10, -10}};
	};
}

