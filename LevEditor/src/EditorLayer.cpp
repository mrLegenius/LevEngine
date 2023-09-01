#include "pch.h"
#include "EditorLayer.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "Selection.h"

namespace LevEngine::Editor
{
    void OnKatamariCollided(Entity me, Entity other)
    {
        /*auto& myCollider = me.GetComponent<SphereCollider>();
        auto& myTransform = me.GetComponent<Transform>();

        auto& otherRigidbody = other.GetComponent<Rigidbody>();
        auto& otherTransform = other.GetComponent<Transform>();
        if (otherRigidbody.bodyType == BodyType::Static) return;

        const auto size = myCollider.radius;
        const auto otherSize = Math::MaxElement(otherTransform.GetWorldScale());

        if (size <= otherSize) return;

        myCollider.radius += otherSize;

        otherRigidbody.enabled = false;

        otherTransform.SetParent(me);*/
    }

    class TestSystem : public System
    {
        void Update(float deltaTime, entt::registry& registry) override
        {
            auto view = registry.view<Transform, DirectionalLightComponent, CameraComponent>();

            for (auto entity : view)
            {
                auto [transform, light, camera] = view.get<Transform, DirectionalLightComponent, CameraComponent>(entity);

                /* auto rot = transform.GetLocalRotation();
                 auto mouseDelta = Input::GetMouseDelta();
                 rot.x += mouseDelta.second / 180;
                 rot.y += mouseDelta.first / 180;
                 transform.SetLocalRotationRadians(rot);*/

                float delta = 0;
                if (Input::IsKeyDown(KeyCode::Q))
                    delta = 0.001f;
                else if (Input::IsKeyDown(KeyCode::E))
                    delta = -0.001f;

                auto ortho = camera.camera.GetOrthographicSize();
                camera.camera.SetOrthographicSize(ortho + delta);
            }
        }
    };
    void EditorLayer::OnAttach()
    {
        LEV_PROFILE_FUNCTION();

        m_Console = CreateRef<ConsolePanel>();
        Log::Logger::AddLogHandler(m_Console);

        AssetDatabase::ProcessAllAssets();

        m_IconPlay = Texture::Create("resources/Icons/PlayButton.png");
        m_IconStop = Texture::Create("resources/Icons/StopButton.png");

        m_ActiveScene = CreateRef<Scene>();

        m_Viewport = CreateRef<ViewportPanel>(Application::Get().GetWindow().GetContext()->GetRenderTarget()->GetTexture(AttachmentPoint::Color0));
        m_Game = CreateRef<GamePanel>(Application::Get().GetWindow().GetContext()->GetRenderTarget()->GetTexture(AttachmentPoint::Color0));
        m_Hierarchy = CreateRef<HierarchyPanel>(m_ActiveScene);
        m_Properties = CreateRef<PropertiesPanel>();
        m_AssetsBrowser = CreateRef<AssetBrowserPanel>();


        //<--- Systems ---<<
        //m_ActiveScene->RegisterLateUpdateSystem(CreateRef<OrbitCameraSystem>());
        //m_ActiveScene->RegisterLateUpdateSystem(CreateRef<KatamariCollisionSystem>());
        //m_ActiveScene->RegisterUpdateSystem(CreateRef<KatamariPlayerSystem>());
        //m_ActiveScene->RegisterUpdateSystem(CreateRef<TestSystem>());
        //m_ActiveScene->RegisterOneFrame<CollisionBeginEvent>();
        //m_ActiveScene->RegisterOneFrame<CollisionEndEvent>();

        Application::Get().GetWindow().EnableCursor();
    }

    void EditorLayer::OnEvent(Event& event)
    {
        EventDispatcher dispatcher{ event };
        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<WindowResizedEvent>(BIND_EVENT_FN(EditorLayer::OnWindowResized));
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
            //if (control)
                //OnDuplicateEntity();

            break;
        }

        default:
            break;
        }

        return m_Viewport->OnKeyPressed(event);
    }

    bool EditorLayer::OnWindowResized(const WindowResizedEvent& e) const
    {
        m_ActiveScene->OnViewportResized(e.GetWidth(), e.GetHeight());
        Renderer::SetViewport(e.GetWidth(), e.GetHeight());
        return false;
    }

    void EditorLayer::OnUpdate(const float deltaTime)
    {
        LEV_PROFILE_FUNCTION();

        if (Input::IsKeyDown(KeyCode::Escape))
        {
            Application::Get().GetWindow().EnableCursor();
            m_Hierarchy->Focus();
        }

        switch (m_SceneState)
        {
	        case SceneState::Edit:
	        {
	            break;
	        }
	        case SceneState::Play:
	        {
	            if (m_Viewport->IsFocused() || m_Game->IsFocused())
	                Application::Get().GetWindow().DisableCursor();

	            m_ActiveScene->OnUpdate(deltaTime);
	            m_ActiveScene->OnPhysics(deltaTime);
	            m_ActiveScene->OnLateUpdate(deltaTime);

	            break;
	        }
        }

        if (m_Viewport->IsActive())
        {
            m_Viewport->UpdateCamera(deltaTime);
            auto& camera = m_Viewport->GetCamera();
            m_ActiveScene->OnRender(&camera, &camera.GetTransform());
            m_Viewport->UpdateTexture(Application::Get().GetWindow().GetContext()->GetRenderTarget()->GetTexture(AttachmentPoint::Color0));
        }

        if (m_Game->IsActive())
        {
            m_ActiveScene->OnRender();
            m_Game->UpdateTexture(Application::Get().GetWindow().GetContext()->GetRenderTarget()->GetTexture(AttachmentPoint::Color0));
        }
    }

    void EditorLayer::OnGUIRender()
    {
        ImGui::ShowDemoWindow(nullptr);

        LEV_PROFILE_FUNCTION();

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

    constexpr float toolbarSize = 10;
    float menuBarHeight;

    void EditorLayer::DrawToolbar()
    {
        static auto selectIcon = Texture::Create("resources/Icons/Select.png");
        static auto translateIcon = Texture::Create("resources/Icons/Translate.png");
        static auto rotationIcon = Texture::Create("resources/Icons/Rotate.png");
        static auto scaleIcon = Texture::Create("resources/Icons/Scale.png");

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + menuBarHeight));
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, toolbarSize));
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        const auto& colors = ImGui::GetStyle().Colors;
        const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
        const auto& buttonActive = colors[ImGuiCol_ButtonActive];
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

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

        ImGui::BeginTable("tools", 4, 0, ImVec2(0, 0), size * 4);
        
        ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, size);
        ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, size);
        ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, size);
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

        ImGui::EndTable();

        size = ImGui::GetWindowHeight() - 4.0f;
        const Ref<Texture> icon = m_SceneState == SceneState::Edit ? m_IconPlay : m_IconStop;
        ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
        ImGui::SetCursorPosY(padding / 2);
        if (ImGui::ImageButton(icon->GetId(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
        {
            if (m_SceneState == SceneState::Edit)
                OnScenePlay();
            else if (m_SceneState == SceneState::Play)
                OnSceneStop();
        }
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(3);
        ImGui::End();
    }

    void EditorLayer::DrawStatusbar()
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, toolbarSize));
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, toolbarSize));
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        const auto& colors = ImGui::GetStyle().Colors;
        const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
        const auto& buttonActive = colors[ImGuiCol_ButtonActive];
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

        constexpr ImGuiWindowFlags windowFlags = 0
            | ImGuiWindowFlags_NoDocking
            | ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoSavedSettings;

        ImGui::Begin("##statusbar", nullptr, windowFlags);
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(3);
        ImGui::End();
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

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, toolbarSize * 3));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.

        ImGui::Begin("Master DockSpace", nullptr, windowFlags);
        ImGuiID dockMain = ImGui::GetID("MyDockspace");
        ImGui::DockSpace(dockMain);
        ImGui::PopStyleVar(3);
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

            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

    void EditorLayer::CreateNewScene()
    {
        m_ActiveScene = CreateRef<Scene>();
        /*m_ActiveScene->OnViewportResized(
            static_cast<uint32_t>(m_Viewport->GetWidth()),
            static_cast<uint32_t>(m_Viewport->GetHeight()));*/
        m_Hierarchy->SetContext(m_ActiveScene);
        m_EditorScenePath = std::filesystem::path();
    }

    void EditorLayer::OpenScene()
    {
        const auto filepath = FileDialogs::OpenFile("LevEngine Scene (*.scene)\0*.scene\0");
        if (!filepath.empty())
        {
            OpenScene(filepath);
        }
    }

    void EditorLayer::OpenScene(const std::filesystem::path& path)
    {
        if (path.extension().string() != ".scene")
        {
            Log::Warning("Failed to open scene. {0} is not a scene file", path.filename().string());
            return;
        }

        if (m_SceneState != SceneState::Edit)
            OnSceneStop();

        const Ref<Scene>& newScene = CreateRef<Scene>();
        SceneSerializer sceneSerializer(newScene);
        if (sceneSerializer.Deserialize(path.generic_string()))
        {
            Selection::Deselect();
            m_EditorScenePath = path;
            m_ActiveScene = newScene;
            m_Hierarchy->SetContext(newScene);

            //TODO: Move this to scene start
            //m_ActiveScene->RegisterLateUpdateSystem(CreateRef<OrbitCameraSystem>());
            //m_ActiveScene->RegisterLateUpdateSystem(CreateRef<KatamariCollisionSystem>());
            //m_ActiveScene->RegisterUpdateSystem(CreateRef<KatamariPlayerSystem>());
            //m_ActiveScene->RegisterUpdateSystem(CreateRef<TestSystem>());
            //m_ActiveScene->RegisterOneFrame<CollisionBeginEvent>();
            //m_ActiveScene->RegisterOneFrame<CollisionEndEvent>();
        }
    }

    bool EditorLayer::SaveScene()
    {
        if (m_SceneState == SceneState::Play)
        {
            Log::CoreWarning("Can't save scene in play mode");
            return false;
        }

        if (!m_EditorScenePath.empty())
        {
            const SceneSerializer sceneSerializer(m_ActiveScene);
            sceneSerializer.Serialize(m_EditorScenePath.string());

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

        const auto filepath = FileDialogs::SaveFile("LevEngine Scene (*.scene)\0*.scene\0");
        if (!filepath.empty())
        {
            const SceneSerializer sceneSerializer(m_ActiveScene);
            sceneSerializer.Serialize(filepath);

            m_EditorScenePath = filepath;
            return true;
        }

        return false;
    }

    void EditorLayer::OnScenePlay()
    {
        if (!SaveScene()) return;

        m_Game->Focus();
        m_SceneState = SceneState::Play;
        Selection::Deselect();
    }

    void EditorLayer::OnSceneStop()
    {
        m_SceneState = SceneState::Edit;
        Selection::Deselect();

        OpenScene(m_EditorScenePath);
    }
}
