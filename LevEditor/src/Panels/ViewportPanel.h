#pragma once
#include "Panel.h"
#include "EditorCamera.h"
#include "Gizmo.h"
#include "Undo/EntityEditTracker.h"

namespace LevEngine::Editor
{
	class ViewportPanel final : public Panel
	{
	public:
		ViewportPanel();

		explicit ViewportPanel(const Ref<Texture>& renderTexture);

		[[nodiscard]] EditorCamera& GetCamera() { return m_Camera; }
		void UpdateCamera(const float deltaTime);
		void FocusCameraOn(Entity entity) { m_Camera.Focus(entity); }

		bool OnKeyPressed(KeyPressedEvent& event) override;

		void UpdateTexture(const Ref<Texture>& renderTexture);

		[[nodiscard]] float GetWidth() const { return m_Size.x; }
		[[nodiscard]] float GetHeight() const { return m_Size.y; }

	protected:
		String GetName() override { return "Viewport"; }
		void DrawContent() override;
		//Returns true when a gizmo was drawn, so its interaction state is the one of this frame
		bool DrawGizmo();

	private:

		void DrawToolbar();
		void HandlePicking(bool gizmoDrawn) const;
		//Left top corner of the whole texture in screen coordinates, the visible part of it is only the center
		[[nodiscard]] Vector2 GetTextureOrigin() const;

		Vector2 m_Size{ 0.0f };
		Vector2 m_Bounds[2];

		Ref<Texture> m_Texture;

		EditorCamera m_Camera{60.0f, 0.1f, 1000.0f, Vector3{0, 10, -10}};

		//<--- One drag of the gizmo is one undo step, however many frames it took ---<<
		EntityEditTracker m_GizmoTracker{ "Transform" };
	};
}

