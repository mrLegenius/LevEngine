#include "pch.h"
#include "EditorLayer.h"

#include "ModalPopup.h"
#include "Project.h"
#include "Selection.h"
#include "Assets/ShaderLibrary.h"

#include "ComponentDebugRenderers/ComponentDebugRenderer.h"
#include "Essentials/MenuBar.h"
#include "Essentials/TitleBar.h"
#include "Panels/AssetBrowserPanel.h"
#include "Panels/ConsoleLog.h"
#include "Panels/ConsolePanel.h"
#include "Panels/DockSpace.h"
#include "Panels/GamePanel.h"
#include "Panels/HierarchyPanel.h"
#include "Panels/PanelManager.h"
#include "Panels/PanelTypes.h"
#include "Panels/PropertiesPanel.h"
#include "Panels/ScriptsPanel.h"
#include "Panels/SettingsPanel.h"
#include "Panels/StatisticsPanel.h"
#include "Panels/StatusBar.h"
#include "Panels/Toolbar.h"
#include "Panels/ViewportPanel.h"
#include "Physics/Physics.h"
#include "Renderer/RenderContext.h"

namespace LevEngine::Editor
{
    namespace
    {
        Ref<Texture> GetMainRenderTexture()
        {
            return App::Get().GetWindow().GetContext()->GetRenderTarget()->GetTexture(AttachmentPoint::Color0);
        }
    }

    void EditorLayer::OnAttach()
    {
        LEV_PROFILE_FUNCTION();

        ConsoleLog::Init();

        m_SaveData.Load();

        m_ProjectEditor = CreateScope<ProjectEditor>(std::bind(&EditorLayer::OnProjectLoaded, this));
        m_SceneEditor = CreateScope<SceneEditor>([this]{ return m_SceneState; });

        //The title bar is the only way to move or close the window, so it exists
        //before any project is loaded
        m_MainTitleBar = CreateRef<TitleBar>();

        m_DockSpace = CreateRef<DockSpace>();
        m_PanelManager = CreateRef<PanelManager>();
        RegisterPanels();

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

    void EditorLayer::RegisterPanels()
    {
        m_PanelManager->RegisterPanelType(PanelTypes::Viewport,
            []() -> Ref<Panel> { return CreateRef<ViewportPanel>(GetMainRenderTexture()); }, true);

        m_PanelManager->RegisterPanelType(PanelTypes::Game,
            [this]() -> Ref<Panel>
            {
                return CreateRef<GamePanel>(GetMainRenderTexture(), [this] { return m_SceneState; });
            }, true);

        m_PanelManager->RegisterPanelType(PanelTypes::Hierarchy,
            [this]() -> Ref<Panel>
            {
                return CreateRef<HierarchyPanel>([this](const Entity entity) { FocusViewportsOn(entity); });
            }, true);

        m_PanelManager->RegisterPanelType(PanelTypes::Properties,
            []() -> Ref<Panel> { return CreateRef<PropertiesPanel>(); }, true);

        m_PanelManager->RegisterPanelType(PanelTypes::AssetBrowser,
            []() -> Ref<Panel> { return CreateRef<AssetBrowserPanel>(); }, true);

        m_PanelManager->RegisterPanelType(PanelTypes::Console,
            []() -> Ref<Panel> { return CreateRef<ConsolePanel>(); }, true);

        m_PanelManager->RegisterPanelType(PanelTypes::Settings,
            []() -> Ref<Panel> { return CreateRef<SettingsPanel>(); }, false);

        m_PanelManager->RegisterPanelType(PanelTypes::Statistics,
            []() -> Ref<Panel> { return CreateRef<StatisticsPanel>(); }, false);

        m_PanelManager->RegisterPanelType(PanelTypes::Scripts,
            []() -> Ref<Panel> { return CreateRef<ScriptsPanel>(); }, true);
    }

    void EditorLayer::FocusViewportsOn(const Entity entity) const
    {
        for (const auto& viewport : m_PanelManager->GetPanelsOfType<ViewportPanel>())
        {
            if (viewport->IsActive())
                viewport->FocusCameraOn(entity);
        }
    }

    void EditorLayer::OnEvent(Event& event)
    {
        EventDispatcher dispatcher{ event };
        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
    }
    void EditorLayer::OnUpdate(const float deltaTime)
    {
        LEV_PROFILE_FUNCTION();

        //Layers are updated before the first ImGui frame, so it is the first place
        //where the ImGui context is already available
        m_PanelManager->Initialize();

        m_ProjectEditor->Update();

        if (!Project::GetProject()) return;

        AssetDatabase::ReimportChangedAssets();
        ShaderLibrary::ReimportChangedAssets();

        SceneManager::TryLoadRequestedScene();

        if (Input::IsKeyDown(KeyCode::Escape))
        {
            for (const auto& game : m_PanelManager->GetPanelsOfType<GamePanel>())
                game->Unfocus();
        }

        const auto& activeScene = SceneManager::GetActiveScene();

        switch (m_SceneState)
        {
        case SceneState::Edit:
            {
                activeScene->DestroyAllMarkedEntities();
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

        for (const auto& viewport : m_PanelManager->GetPanelsOfType<ViewportPanel>())
        {
            if (viewport->IsActive())
                viewport->UpdateCamera(deltaTime);
        }
    }
    void EditorLayer::OnRender()
    {
        if (!Project::GetProject()) return;

        const auto& activeScene = SceneManager::GetActiveScene();

        //Every viewport has its own camera, so the scene is rendered once per viewport
        for (const auto& viewport : m_PanelManager->GetPanelsOfType<ViewportPanel>())
        {
            if (!viewport->IsActive()) continue;

            DebugRender::DrawGrid(Vector3::Zero, Vector3::Right, Vector3::Forward, 100, 100, 1.0f, Color::Gray);

            DoComponentRenderDebug();

            auto& camera = viewport->GetCamera();
            activeScene->OnRender(&camera, &camera.GetTransform());
            viewport->UpdateTexture(GetMainRenderTexture());
        }

        //Game panels all show the main camera, so one render is enough for all of them
        const auto gamePanels = m_PanelManager->GetPanelsOfType<GamePanel>();
        const bool hasActiveGamePanel = eastl::any_of(gamePanels.begin(), gamePanels.end(),
            [](const Ref<GamePanel>& panel) { return panel->IsActive(); });

        if (hasActiveGamePanel)
        {
            activeScene->OnRender();

            const auto mainTexture = GetMainRenderTexture();
            for (const auto& game : gamePanels)
            {
                if (game->IsActive())
                    game->UpdateTexture(mainTexture);
            }
        }
    }
    void EditorLayer::OnGUIRender()
    {
        LEV_PROFILE_FUNCTION();

        //ImGui::ShowDemoWindow(nullptr);

        const bool hasCustomTitleBar = App::Get().GetWindow().HasCustomTitleBar();
        if (hasCustomTitleBar)
            m_MainTitleBar->Render(m_MainMenuBar, GetWindowTitle());

        ModalPopup::Render();

        if (!Project::GetProject()) return;

        m_PanelManager->EnsureRestored();

        m_DockSpace->Render(*m_PanelManager);

        //With a custom title bar the menus are drawn inside it
        if (!hasCustomTitleBar)
            m_MainMenuBar->RenderAsMain();

        m_MainToolbar->Render();
        m_MainStatusBar->Render();
        m_PanelManager->Render();
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent& event)
    {
        if (event.GetRepeatCount() > 0)
            return false;

        if (m_SceneEditor->OnKeyPressed(event))
            return true;

        if (m_PanelManager->OnKeyPressed(event))
            return true;

        const bool control = Input::IsKeyDown(KeyCode::LeftControl) ||
            Input::IsKeyDown(KeyCode::RightControl);

        switch (event.GetKeyCode())
        {
            case KeyCode::P:
            {
                if (!control) break;

                if (m_SceneState == SceneState::Play)
                    OnSceneStop();
                else
                    OnScenePlay();

                return true;
            }
        }

        return false;
    }

    void EditorLayer::OnScenePlay()
    {
        if (!m_SceneEditor->SaveScene()) return;

        App::Get().IsPlaying = true;

        App::Get().GetPhysics().ResetPhysicsScene();
        App::Get().GetPhysics().ClearAccumulator();

        SceneManager::LoadScene(SceneManager::GetActiveScenePath());

        m_SceneState = SceneState::Play;

        m_PanelManager->OpenOrFocusPanel(PanelTypes::Game);

        Selection::Deselect();
    }
    void EditorLayer::OnSceneStop()
    {
        App::Get().IsPlaying = false;

        m_SceneState = SceneState::Edit;

        for (const auto& game : m_PanelManager->GetPanelsOfType<GamePanel>())
            game->Unfocus();

        Selection::Deselect();

        m_SceneEditor->OpenScene(SceneManager::GetActiveScenePath());
    }

    void EditorLayer::DoComponentRenderDebug()
    {
        SceneManager::GetActiveScene()->ForEachEntityUnordered(
            [](const Entity entity)
            {
                for (const auto debugRenderers : ClassCollection<IComponentDebugRenderer>::Instance())
                {
                    debugRenderers->Draw(entity);
                }
            });
    }
    String EditorLayer::GetWindowTitle()
    {
        const auto& specification = App::Get().GetSpecification();

        if (!Project::GetProject())
            return specification.Name;

        const auto projectName = ToString(Project::GetPath().stem());
        const auto scenePath = SceneManager::GetActiveScenePath();

        if (scenePath.empty())
            return Format("{0} - {1}", specification.Name, projectName);

        return Format("{0} - {1} - {2}", specification.Name, projectName, ToString(scenePath.stem()));
    }

    void EditorLayer::OnProjectLoaded()
    {
        m_SaveData.SetLastOpenedProject(Project::GetPath());
        m_SaveData.Save();

        AssetDatabase::ProcessAllAssets();
        ResourceManager::Init(Project::GetRoot());

        const auto startScene = Project::GetStartScene();
        if (startScene.empty() || !m_SceneEditor->OpenScene(startScene))
            SceneManager::LoadEmptyScene();

        m_MainStatusBar = CreateRef<StatusBar>();
        m_MainMenuBar = CreateRef<MenuBar>();
        m_MainToolbar = CreateRef<Toolbar>(m_MainMenuBar, [this]{ return m_SceneState; }, std::bind(&EditorLayer::OnPlayButtonClicked, this));

        m_SceneEditor->AddMainMenuItems(m_MainMenuBar);
        m_ProjectEditor->AddMainMenuItems(m_MainMenuBar);
        m_PanelManager->AddMainMenuItems(m_MainMenuBar);

        m_MainMenuBar->AddMenuItem("Window/Reset Layout", String(),
            [this] { m_PanelManager->OpenDefaultPanels(); });

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
