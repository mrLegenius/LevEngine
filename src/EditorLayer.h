#pragma once
#include "LevEngine.h"
#include "Panels/SceneHierarchy.h"
#include "Panels/AssetsBrowser.h"
#include "src/Events/KeyEvent.h"

namespace LevEngine
{
	class EditorLayer final : public Layer
	{
	public:
		EditorLayer();
		~EditorLayer() override = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate() override;
		void OnEvent(Event& event) override;
		void OnImGuiRender() override;

	private:
		bool OnKeyPressed(KeyPressedEvent& event);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& event);

		void CreateNewScene();
		void OpenScene();
        void OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveSceneAs();

        void OnScenePlay();
        void OnSceneStop();

        void OnDuplicateEntity();
		
		void DrawDockSpace();
		void DrawViewport();
		void DrawStatistics();
        void DrawToolbar();

        Entity GetHoveredEntity();

		EditorCamera m_EditorCamera;
		
		Ref<Framebuffer> m_Framebuffer = nullptr;

		Ref<Scene> m_ActiveScene = nullptr;
        Ref<Scene> m_EditorScene = nullptr;

        std::filesystem::path m_EditorScenePath;

		Entity m_HoveredEntity;

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec2 m_ViewportSize { 0.0f };
		glm::vec2 m_ViewportBounds[2];

		int m_GizmoType = -1;

        enum class SceneState
        {
            Edit = 0, Play = 1
        };
        SceneState m_SceneState = SceneState::Edit;

        SceneHierarchy m_Hierarchy;
        AssetsBrowser m_AssetsBrowser;

        Ref<Texture2D> m_IconPlay, m_IconStop;

        void OnOverlayRender();
    };
}
