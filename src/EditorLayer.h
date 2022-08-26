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

		
		void DrawDockSpace();
		void DrawViewport();
		void DrawStatistics();

        Entity GetHoveredEntity();

		EditorCamera m_EditorCamera;
		
		Ref<Framebuffer> m_Framebuffer = nullptr;

		Ref<Scene> m_ActiveScene = nullptr;
		std::string m_ActiveScenePath;

		Entity m_HoveredEntity;

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec2 m_ViewportSize { 0.0f };
		glm::vec2 m_ViewportBounds[2];

		int m_GizmoType = -1;

        SceneHierarchy m_Hierarchy;
        AssetsBrowser m_AssetsBrowser;
    };
}
