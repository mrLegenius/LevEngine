#include "pch.h"
#include "EditorLayer.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "EntitySelection.h"
#include "ModalPopup.h"
#include "Project.h"
#include "Selection.h"
#include "Scene/Systems/Animation/WaypointDisplacementByTimeSystem.h"
#include "Scene/Systems/Animation/WaypointPositionUpdateSystem.h"
#include "GUI/ScopedGUIHelpers.h"

namespace LevEngine::Editor
{
    void EditorLayer::ShowProjectSelectionPopup()
    {
        ModalPopup::Show("Project Selection",
    "Open existing or create new project",
    "Open",
    "Create new",
    [this] { if(!OpenProject()) ShowProjectSelectionPopup(); },
    [this]{ if(!NewProject()) ShowProjectSelectionPopup(); });
    }
    
    bool EditorLayer::OpenProject()
    {
        const String& path = FileDialogs::OpenFile("LevProject (*.levproject)\0*.levproject\0");

        if(Project::Load(path.c_str()))
        {
            LoadProject();
            return true;
        }

        return false;
    }

    bool EditorLayer::NewProject()
    {
        const String& path = FileDialogs::SaveFile("LevProject (*.levproject)\0*.levproject\0", "levproject");

        if(Project::CreateNew(path.c_str()))
        {
            LoadProject();
            return true;
        }
        
        return false;
    }


    void EditorLayer::LoadProject()
    {
        m_SaveData.SetLastOpenedProject(Project::GetPath());
        m_SaveData.Save();

        ResourceManager::Init(Project::GetRoot());
        AssetDatabase::ProcessAllAssets();

        const auto startScene = Project::GetStartScene();
        if (startScene.empty() || !OpenScene(startScene))
            SceneManager::LoadEmptyScene();
        
        m_Viewport = CreateRef<ViewportPanel>(Application::Get().GetWindow().GetContext()->GetRenderTarget()->GetTexture(AttachmentPoint::Color0));
        m_Game = CreateRef<GamePanel>(Application::Get().GetWindow().GetContext()->GetRenderTarget()->GetTexture(AttachmentPoint::Color0));
        m_Hierarchy = CreateRef<HierarchyPanel>();
        m_Properties = CreateRef<PropertiesPanel>();
        m_AssetsBrowser = CreateRef<AssetBrowserPanel>();
    }
    
    void EditorLayer::OnAttach()
    {
        LEV_PROFILE_FUNCTION();

        //spdlog uses shared_ptr so we use it here as well
        m_Console = std::make_shared<ConsolePanel>();
        Log::Logger::AddLogHandler(m_Console);

        m_SaveData.Load();

        if (Project::Load(m_SaveData.GetLastOpenedProject()))
        {
            LoadProject();
        }
        else
        {
            ShowProjectSelectionPopup();
        }
        
        Application::Get().GetWindow().EnableCursor();
    }

    void EditorLayer::OnEvent(Event& event)
    {
        EventDispatcher dispatcher{ event };
        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<WindowResizedEvent>(BIND_EVENT_FN(EditorLayer::OnWindowResized));
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
        
        DrawDockSpace();
        m_Viewport->Render();
        m_Hierarchy->Render();
        m_Properties->Render();
        m_AssetsBrowser->Render();
        m_Game->Render();
        m_Console->Render();
        //DrawStatistics();
        DrawToolbar();
        DrawStatusbar();
    }

    void EditorLayer::OnDuplicateEntity()
    {
        if (const auto entitySelection = Selection::CurrentAs<EntitySelection>())
        {
            SceneManager::GetActiveScene()->DuplicateEntity(entitySelection->Get());
        }
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent& event)
    {
        //Shortcuts
        if (event.GetRepeatCount() > 0)
            return false;

        const bool control = Input::IsKeyDown(KeyCode::LeftControl) ||
            Input::IsKeyDown(KeyCode::RightControl);
        const bool shift = Input::IsKeyDown(KeyCode::LeftShift) ||
            Input::IsKeyDown(KeyCode::RightShift);
        const bool alt = Input::IsKeyDown(KeyCode::LeftAlt) ||
            Input::IsKeyDown(KeyCode::RightAlt);

        switch (event.GetKeyCode())
        {

        case KeyCode::N:
            {
                if (control) { CreateNewScene(); }
                break;
            }
        case KeyCode::O:
            {
                if (control) { OpenScene(); }
                break;
            }
        case KeyCode::S:
            {
                if (control && shift) { SaveSceneAs(); }
                else if (control) { SaveScene(); }
                break;
            }
        case KeyCode::D:
            {
                if (control)
                    OnDuplicateEntity();

                break;
            }

        default:
            break;
        }

        m_Hierarchy->OnKeyPressed(event);
        return m_Viewport->OnKeyPressed(event);
    }

    bool EditorLayer::OnWindowResized(const WindowResizedEvent& e) const
    {
        const auto height = e.GetHeight();
        const auto width = e.GetWidth();
        
        if (width == 0 || height == 0) return false;

        SceneManager::GetActiveScene()->OnViewportResized(e.GetWidth(), e.GetHeight());
        Renderer::SetViewport(static_cast<float>(width), static_cast<float>(height));
        return false;
    }


    constexpr float toolbarSize = 10;
    float menuBarHeight;

    void EditorLayer::DrawToolbar()
    {
        static auto selectIcon = Icons::Select();
        static auto translateIcon = Icons::Translate();
        static auto rotationIcon = Icons::Rotate();
        static auto scaleIcon = Icons::Scale();

        static auto iconPlay = Icons::Play();
        static auto iconStop = Icons::Stop();

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + menuBarHeight));
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, toolbarSize));
        ImGui::SetNextWindowViewport(viewport->ID);

        GUI::ScopedVariable windowPadding(ImGuiStyleVar_WindowPadding, Vector2(0, 2));
        GUI::ScopedVariable itemInnerSpacing(ImGuiStyleVar_ItemInnerSpacing, Vector2::Zero);
        GUI::ScopedVariable windowBorderSize(ImGuiStyleVar_WindowBorderSize, 0);

        GUI::ScopedColor buttonColor(ImGuiCol_Button, Color::Clear);
        const auto& colors = ImGui::GetStyle().Colors;
        const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
        GUI::ScopedColor buttonHoveredColor(ImGuiCol_ButtonHovered, Color(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
        const auto& buttonActive = colors[ImGuiCol_ButtonActive];
        GUI::ScopedColor buttonActiveColor(ImGuiCol_ButtonActive, Color(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));
        const ImGuiIO& io = ImGui::GetIO();
        const auto boldFont = io.Fonts->Fonts[0];
        
        constexpr ImGuiWindowFlags windowFlags = 0
            | ImGuiWindowFlags_NoDocking
            | ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoSavedSettings;

        ImGui::Begin("##toolbar", nullptr, windowFlags);

        constexpr auto padding = 4;
        float size = 20;

        ImGui::SetCursorPosX(10);
        ImGui::SetCursorPosY(padding);

        constexpr int columns = 7;
        ImGui::BeginTable("tools", columns, 0, ImVec2(0, 0), size * 4);

        for (int i = 0; i < columns; ++i)
            ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, size);

        {
            ImGui::TableNextColumn();
            const auto cursorPos = ImGui::GetCursorPos();
            if (ImGui::Selectable("##tool_none", Gizmo::Tool == Gizmo::ToolType::None, 0, ImVec2(size, size)))
                Gizmo::Tool = Gizmo::ToolType::None;
            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(cursorPos);
            ImGui::Image(selectIcon->GetId(), ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0));
        }

        {
            ImGui::TableNextColumn();
            const auto cursorPos = ImGui::GetCursorPos();
            if (ImGui::Selectable("##tool_translate", Gizmo::Tool == Gizmo::ToolType::Translate, 0, ImVec2(size, size)))
                Gizmo::Tool = Gizmo::ToolType::Translate;
            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(cursorPos);
            ImGui::Image(translateIcon->GetId(), ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0));
        }

        {
            ImGui::TableNextColumn();
            const auto cursorPos = ImGui::GetCursorPos();
            if (ImGui::Selectable("##tool_rotate", Gizmo::Tool == Gizmo::ToolType::Rotate, 0, ImVec2(size, size)))
                Gizmo::Tool = Gizmo::ToolType::Rotate;
            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(cursorPos);
            ImGui::Image(rotationIcon->GetId(), ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0));
        }

        {
            ImGui::TableNextColumn();
            const auto cursorPos = ImGui::GetCursorPos();
            if (ImGui::Selectable("##tool_scale", Gizmo::Tool == Gizmo::ToolType::Scale, 0, ImVec2(size, size)))
                Gizmo::Tool = Gizmo::ToolType::Scale;
            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(cursorPos);
            ImGui::Image(scaleIcon->GetId(), ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0));
        }

        {
            ImGui::TableNextColumn();
            ImGui::Dummy(ImVec2(size, size));
        }
        
        {
            ImGui::TableNextColumn();
            const auto cursorPos = ImGui::GetCursorPos();
            if (ImGui::Selectable("##tool_space_world", Gizmo::Space == Gizmo::ToolSpace::World, 0, ImVec2(size, size)))
                Gizmo::Space = Gizmo::ToolSpace::World;
            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(cursorPos);
            
            ImGui::PushFont(boldFont);

            const auto width = ImGui::GetColumnWidth();
            const auto textWidth = ImGui::CalcTextSize("W").x;
            const auto cursorPosX = cursorPos.x + (width - textWidth) * 0.5f;
            ImGui::SetCursorPosX(cursorPosX);
            
            ImGui::Text("W");
            ImGui::PopFont();
        }

        {
            ImGui::TableNextColumn();
            const auto cursorPos = ImGui::GetCursorPos();
            if (ImGui::Selectable("##tool_space_local", Gizmo::Space == Gizmo::ToolSpace::Local, 0, ImVec2(size, size)))
                Gizmo::Space = Gizmo::ToolSpace::Local;
            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(cursorPos);
            ImGui::PushFont(boldFont);

            const auto width = ImGui::GetColumnWidth();
            const auto textWidth = ImGui::CalcTextSize("L").x;
            const auto cursorPosX = cursorPos.x + (width - textWidth) * 0.5f;
            ImGui::SetCursorPosX(cursorPosX);
            
            ImGui::Text("L");
            ImGui::PopFont();
        }

        ImGui::EndTable();

        size = ImGui::GetWindowHeight() - 4.0f;
        const Ref<Texture> icon = m_SceneState == SceneState::Edit ? iconPlay : iconStop;
        ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
        ImGui::SetCursorPosY(padding / 2);
        if (ImGui::ImageButton(icon->GetId(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
        {
            if (m_SceneState == SceneState::Edit)
                OnScenePlay();
            else if (m_SceneState == SceneState::Play)
                OnSceneStop();
        }
        ImGui::End();
    }

    void EditorLayer::DrawStatusbar()
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, toolbarSize));
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, toolbarSize));
        ImGui::SetNextWindowViewport(viewport->ID);

        GUI::ScopedVariable windowPadding(ImGuiStyleVar_WindowPadding, Vector2(0, 2));
        GUI::ScopedVariable itemInnerSpacing(ImGuiStyleVar_ItemInnerSpacing, Vector2::Zero);
        GUI::ScopedVariable windowBorderSize(ImGuiStyleVar_WindowBorderSize, 0);
        
        GUI::ScopedColor buttonColor(ImGuiCol_Button, Color::Clear);
        const auto& colors = ImGui::GetStyle().Colors;
        const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
        GUI::ScopedColor buttonHoveredColor(ImGuiCol_ButtonHovered, Color(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
        const auto& buttonActive = colors[ImGuiCol_ButtonActive];
        GUI::ScopedColor buttonActiveColor(ImGuiCol_ButtonActive, Color(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));
        
        constexpr ImGuiWindowFlags windowFlags = 0
            | ImGuiWindowFlags_NoDocking
            | ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoSavedSettings;

        ImGui::Begin("##statusbar", nullptr, windowFlags);
        ImGui::End();
    }

    void EditorLayer::SetCurrentSceneAsStartScene() const
    {
        if (!SceneManager::GetActiveScene())
        {
            Log::CoreWarning("There is no active scene");
            return;
        }

        Project::SetStartScene(SceneManager::GetActiveScenePath());
        Project::Save();
    }

    void EditorLayer::DrawDockSpace()
    {
        LEV_PROFILE_FUNCTION();

        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y));
        ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, viewport->WorkSize.y));
        ImGui::SetNextWindowViewport(viewport->ID);

        constexpr ImGuiWindowFlags windowFlags = 0
            | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
            | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        
        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        {
            GUI::ScopedVariable windowPadding(ImGuiStyleVar_WindowPadding, Vector2(0, toolbarSize * 3));
            GUI::ScopedVariable windowRounding(ImGuiStyleVar_WindowRounding, 0.0f);
            GUI::ScopedVariable windowBorderSize(ImGuiStyleVar_WindowBorderSize, 0);
            
            ImGui::Begin("Master DockSpace", nullptr, windowFlags);
            ImGuiID dockMain = ImGui::GetID("MyDockspace");
            ImGui::DockSpace(dockMain);
        }
        // Save off menu bar height for later.
        menuBarHeight = ImGui::GetCurrentWindow()->MenuBarHeight();

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Scene", "Ctrl+N"))
                    CreateNewScene();

                if (ImGui::MenuItem("Open Scene...", "Ctrl+O"))
                    OpenScene();

                if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
                    SaveScene();

                if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
                    SaveSceneAs();

                if (ImGui::MenuItem("Exit"))
                    Application::Get().Close();

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Project"))
            {
                if (ImGui::MenuItem("Open project..."))
                    if (!OpenProject())
                        Log::CoreWarning("Failed to open project");

                if (ImGui::MenuItem("Create New..."))
                    if(!NewProject())
                        Log::CoreWarning("Failed to open project");
                
                if (ImGui::MenuItem("Set current scene as start scene"))
                    SetCurrentSceneAsStartScene();

                if (ImGui::MenuItem("Build"))
                    Project::Build();

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

    void EditorLayer::CreateNewScene()
    {
        SceneManager::LoadEmptyScene();
    }

    void EditorLayer::OpenScene()
    {
        const auto filepath = FileDialogs::OpenFile("LevEngine Scene (*.scene)\0*.scene\0");
        if (!filepath.empty())
        {
            OpenScene(filepath.c_str());
        }
    }

    bool EditorLayer::OpenScene(const Path& path)
    {
        if (path.extension().string() != ".scene")
        {
            Log::Warning("Failed to open scene. {0} is not a scene file", path.filename().string());
            return false;
        }

        if (m_SceneState != SceneState::Edit)
            OnSceneStop();
        
        if (SceneManager::LoadScene(path))
        {
            Selection::Deselect();
            return true;
        }

        return false;
    }

    bool EditorLayer::SaveScene()
    {
        if (m_SceneState == SceneState::Play)
        {
            Log::CoreWarning("Can't save scene in play mode");
            return false;
        }

        const auto scenePath = SceneManager::GetActiveScenePath();
        if (!scenePath.empty())
        {
            SceneManager::SaveScene(scenePath.string().c_str());
            return true;
        }

        return SaveSceneAs();
    }

    bool EditorLayer::SaveSceneAs()
    {
        if (m_SceneState == SceneState::Play)
        {
            Log::CoreWarning("Can't save scene in play mode");
            return false;
        }

        const auto filepath = FileDialogs::SaveFile("LevEngine Scene (*.scene)\0*.scene\0", "scene");
        if (!filepath.empty())
        {
            SceneManager::SaveScene(filepath);
            return true;
        }

        return false;
    }

    void EditorLayer::OnScenePlay()
    {
        if (!SaveScene()) return;

        auto& scene = SceneManager::GetActiveScene();
        scene->RegisterUpdateSystem<WaypointDisplacementByTimeSystem>();
        scene->RegisterUpdateSystem<WaypointPositionUpdateSystem>();

        m_Game->Focus();
        m_SceneState = SceneState::Play;
        Selection::Deselect();
    }

    void EditorLayer::OnSceneStop()
    {
        m_SceneState = SceneState::Edit;
        Selection::Deselect();

        OpenScene(SceneManager::GetActiveScenePath());
    }
}
