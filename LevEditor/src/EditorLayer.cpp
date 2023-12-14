#include "pch.h"
#include "EditorLayer.h"

#include "ModalPopup.h"
#include "Project.h"
#include "Selection.h"

#include "ComponentDebugRenderers/ComponentDebugRenderer.h"
#include "Essentials/MenuBar.h"
#include "Panels/AssetBrowserPanel.h"
#include "Panels/ConsolePanel.h"
#include "Panels/DockSpace.h"
#include "Panels/GamePanel.h"
#include "Panels/HierarchyPanel.h"
#include "Panels/PropertiesPanel.h"
#include "Panels/ScriptsPanel.h"
#include "Panels/SettingsPanel.h"
#include "Panels/StatisticsPanel.h"
#include "Panels/StatusBar.h"
#include "Panels/Toolbar.h"
#include "Panels/ViewportPanel.h"
#include "Renderer/RendererContext.h"
#include "Scripting/ScriptingManager.h"

namespace LevEngine::Editor
{
    void EditorLayer::OnAttach()
    {
        LEV_PROFILE_FUNCTION();

        //spdlog uses shared_ptr so we use it here as well
        m_Console = std::make_shared<ConsolePanel>();
        Log::Logger::AddLogHandler(m_Console);

        m_SaveData.Load();

        m_ProjectEditor = CreateScope<ProjectEditor>(std::bind(&EditorLayer::OnProjectLoaded, this));
        m_SceneEditor = CreateScope<SceneEditor>([this]{ return m_SceneState; });

        if (Project::Load(m_SaveData.GetLastOpenedProject()))
        {
            OnProjectLoaded();
        }
        else
        {
            m_ProjectEditor->ShowProjectSelectionPopup();
        }
        
        Application::Get().GetWindow().EnableCursor();
    }
    void EditorLayer::OnEvent(Event& event)
    {
        EventDispatcher dispatcher{ event };
        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
    }
    void EditorLayer::OnUpdate(const float deltaTime)
    {
        LEV_PROFILE_FUNCTION();

        if (!Project::GetProject()) return;
        
        if (Input::IsKeyDown(KeyCode::Escape))
        {
            Application::Get().GetWindow().EnableCursor();
            m_Hierarchy->Focus();
        }

        const auto& activeScene = SceneManager::GetActiveScene();
        
        switch (m_SceneState)
        {
	        case SceneState::Edit:
	        {
	            break;
	        }
	        case SceneState::Play:
	        {
	            activeScene->OnUpdate(deltaTime);
	            activeScene->OnPhysics(deltaTime);
	            activeScene->OnLateUpdate(deltaTime);

	            break;
	        }
        }

        if (m_Viewport->IsActive())
            m_Viewport->UpdateCamera(deltaTime);
    }
    void EditorLayer::OnRender()
    {
        if (!Project::GetProject()) return;
        
        const auto& activeScene = SceneManager::GetActiveScene();
        
        if (m_Viewport->IsActive())
        {
            DebugRender::DrawGrid(Vector3::Zero, Vector3::Right, Vector3::Forward, 100, 100, 1.0f, Color::Gray);

            DoComponentRenderDebug();
            
            auto& camera = m_Viewport->GetCamera();
            activeScene->OnRender(&camera, &camera.GetTransform());
            m_Viewport->UpdateTexture(Application::Get().GetWindow().GetContext()->GetRenderTarget()->GetTexture(AttachmentPoint::Color0));
        }

        if (m_Game->IsActive())
        {
            activeScene->OnRender();
            m_Game->UpdateTexture(Application::Get().GetWindow().GetContext()->GetRenderTarget()->GetTexture(AttachmentPoint::Color0));
        }
    }
    void EditorLayer::OnGUIRender()
    {
        LEV_PROFILE_FUNCTION();
        
        ImGui::ShowDemoWindow(nullptr);
        
        ModalPopup::Render();

        if (!Project::GetProject()) return;
        
        m_DockSpace->Render();
        m_Viewport->Render();
        m_Hierarchy->Render();
        m_Properties->Render();
        m_AssetsBrowser->Render();
        m_Game->Render();
        m_Console->Render();
        m_Settings->Render();
        m_Statistics->Render();
        m_ScriptsPanel->Render();
        m_MainToolbar->Render();
        m_MainStatusBar->Render();
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent& event) const
    {
        if (event.GetRepeatCount() > 0)
            return false;

        if (m_SceneEditor->OnKeyPressed(event))
            return true;
        
         if (m_Hierarchy->OnKeyPressed(event))
             return true;

        if (m_Viewport->OnKeyPressed(event))
            return true;
        
        return false;
    }
    
    void EditorLayer::OnScenePlay()
    {
        if (!m_SceneEditor->SaveScene()) return;

        auto& scene = SceneManager::GetActiveScene();        
        
        m_Game->Focus();
        m_SceneState = SceneState::Play;
        scene->OnInit();
        Selection::Deselect();
    }
    void EditorLayer::OnSceneStop()
    {
        m_SceneState = SceneState::Edit;
        Selection::Deselect();
        
        m_SceneEditor->OpenScene(SceneManager::GetActiveScenePath());
    }
    
    void EditorLayer::DoComponentRenderDebug()
    {
        SceneManager::GetActiveScene()->ForEachEntity(
            [](const Entity entity)
            {
                for (const auto debugRenderers : ClassCollection<IComponentDebugRenderer>::Instance())
                {
                    debugRenderers->Draw(entity);
                }
            });
    }
    void EditorLayer::OnProjectLoaded()
    {
        m_SaveData.SetLastOpenedProject(Project::GetPath());
        m_SaveData.Save();

        ResourceManager::Init(Project::GetRoot());
        AssetDatabase::ProcessAllAssets();
        Application::Get().GetScriptingManager().LoadScripts();
        
        const auto startScene = Project::GetStartScene();
        if (startScene.empty() || !m_SceneEditor->OpenScene(startScene))
            SceneManager::LoadEmptyScene();
        
        m_Viewport = CreateRef<ViewportPanel>(Application::Get().GetWindow().GetContext()->GetRenderTarget()->GetTexture(AttachmentPoint::Color0));
        m_Game = CreateRef<GamePanel>(Application::Get().GetWindow().GetContext()->GetRenderTarget()->GetTexture(AttachmentPoint::Color0));
        m_Hierarchy = CreateRef<HierarchyPanel>();
        m_Properties = CreateRef<PropertiesPanel>();
        m_AssetsBrowser = CreateRef<AssetBrowserPanel>();
        m_Settings = CreateRef<SettingsPanel>();
        m_MainStatusBar = CreateRef<StatusBar>();
        m_MainMenuBar = CreateRef<MenuBar>();
        m_MainToolbar = CreateRef<Toolbar>(m_MainMenuBar, [this]{ return m_SceneState; }, std::bind(&EditorLayer::OnPlayButtonClicked, this));
        m_DockSpace = CreateRef<DockSpace>(m_MainToolbar, m_MainMenuBar);
        m_Statistics = CreateRef<StatisticsPanel>();
        m_ScriptsPanel = CreateRef<ScriptsPanel>();

        m_SceneEditor->AddMainMenuItems(m_MainMenuBar);
        m_ProjectEditor->AddMainMenuItems(m_MainMenuBar);
        
        m_MainMenuBar->AddMenuItem("File/Exit", String(), [this] { Application::Get().Close();});
    }
    void EditorLayer::OnPlayButtonClicked()
    {
        if (m_SceneState == SceneState::Edit)
            OnScenePlay();
        else if (m_SceneState == SceneState::Play)
            OnSceneStop();
    }
}
