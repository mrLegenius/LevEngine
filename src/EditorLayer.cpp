#include "EditorLayer.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "glm/gtc/type_ptr.hpp"

namespace LevEngine
{
    extern const std::filesystem::path g_AssetsPath;

    EditorLayer::EditorLayer() : Layer("EditorLayer") { }

    void EditorLayer::OnAttach()
    {
        LEV_PROFILE_FUNCTION();

        FramebufferSpecification fbSpec;
        fbSpec.attachmentSpecification = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RedInteger, FramebufferTextureFormat::Depth };
        fbSpec.width = Application::Get().GetWindow().GetWidth();
        fbSpec.height = Application::Get().GetWindow().GetHeight();

        m_Framebuffer = Framebuffer::Create(fbSpec);

        m_ActiveScene = CreateRef<Scene>();

        m_EditorCamera = EditorCamera(30.0f, 16.0f / 9.0f, 0.1f, 1000.0f);

        m_Hierarchy.SetContext(m_ActiveScene);

        //SceneSerializer sceneSerializer(m_ActiveScene);
        //m_ActiveScenePath = "assets/scenes/BasicScene.scene";
        //sceneSerializer.Deserialize(m_ActiveScenePath);
    }

    void EditorLayer::OnDetach()
    {
        LEV_PROFILE_FUNCTION();
    }

    void EditorLayer::OnUpdate()
    {
        LEV_PROFILE_FUNCTION();
    	
        const auto width = static_cast<uint32_t>(m_ViewportSize.x);
        const auto height = static_cast<uint32_t>(m_ViewportSize.y);

        Renderer2D::ResetStats();
    	
        if(const FramebufferSpecification spec = m_Framebuffer->GetSpecification();
            m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
            (spec.width != width || spec.height != height))
        {
            m_Framebuffer->Resize(width, height);
            m_ActiveScene->OnViewportResized(width, height);
            m_EditorCamera.SetViewportSize(width, height);
        }
    	
        m_EditorCamera.OnUpdate();
    	
        m_Framebuffer->Bind();
        const float gray = 0.69f / 5;
        RenderCommand::SetClearColor(glm::vec4(glm::vec3(gray), 1.0f));
        RenderCommand::Clear();
    	
        // Clear entity ID attachment to -1
        m_Framebuffer->ClearAttachment(1, -1);
    	
        m_ActiveScene->OnUpdateEditor(m_EditorCamera);

        m_HoveredEntity = GetHoveredEntity();

        //COOL GRADIENT COLOR
        //glm::vec4 c{ (i + 3.0f) / 6.0f, (i + 5.0f) / 10.0f, (i + 10.0f) / 15.0f, 1.0f };
        m_Framebuffer->Unbind();
    }

    Entity EditorLayer::GetHoveredEntity() {
        auto[mx, my] = ImGui::GetMousePos();
        mx -= m_ViewportBounds[0].x;
        my -= m_ViewportBounds[0].y;
        const glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
        my = viewportSize.y - my;
        const int mouseX = static_cast<int>(mx);
        const int mouseY = static_cast<int>(my);

        if (mouseX >= 0 && mouseY >= 0 && mouseX < static_cast<int>(viewportSize.x) && mouseY < static_cast<int>(viewportSize.y))
        {
            int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
            return pixelData == -1 ? Entity() : Entity(static_cast<entt::entity>(pixelData), m_ActiveScene.get());
        }

        return {};
    }

    void EditorLayer::OnEvent(Event& event)
    {
        m_EditorCamera.OnEvent(event);

        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(LEV_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<MouseButtonPressedEvent>(LEV_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
    }

    void EditorLayer::OnImGuiRender()
    {
        LEV_PROFILE_FUNCTION();

        DrawDockSpace();
        DrawViewport();	
        DrawStatistics();
        m_Hierarchy.OnImGuiRender();
        m_AssetsBrowser.OnImGuiRender();
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

    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& event)
    {
        if (event.GetMouseButton() == MouseButton::Left)
        {
            if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(KeyCode::LeftAlt))
                m_Hierarchy.SetSelectedEntity(m_HoveredEntity);
        }
           
        return false;
    }

    void EditorLayer::CreateNewScene()
    {
        m_ActiveScene = CreateRef<Scene>();
        m_ActiveScene->OnViewportResized(
            static_cast<uint32_t>(m_ViewportSize.x),
            static_cast<uint32_t>(m_ViewportSize.y));
        m_Hierarchy.SetContext(m_ActiveScene);
        m_ActiveScenePath.clear();
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
        m_ActiveScene = CreateRef<Scene>();
        m_ActiveScene->OnViewportResized(
                static_cast<uint32_t>(m_ViewportSize.x),
                static_cast<uint32_t>(m_ViewportSize.y));
        m_Hierarchy.SetContext(m_ActiveScene);

        SceneSerializer sceneSerializer(m_ActiveScene);
        sceneSerializer.Deserialize(path.generic_string());

        m_ActiveScenePath = path.generic_string();
    }

    void EditorLayer::SaveScene()
    {
        if (!m_ActiveScenePath.empty())
        {
            SceneSerializer sceneSerializer(m_ActiveScene);
            sceneSerializer.Serialize(m_ActiveScenePath);
        }
        else
            SaveSceneAs();
    }

    void EditorLayer::SaveSceneAs()
    {
        const auto filepath = FileDialogs::SaveFile("LevEngine Scene (*.scene)\0*.scene\0");
        if (!filepath.empty())
        {
            SceneSerializer sceneSerializer(m_ActiveScene);
            sceneSerializer.Serialize(filepath);
        	
            m_ActiveScenePath = filepath;
        }
    }

    void EditorLayer::DrawViewport()
    {
        LEV_PROFILE_FUNCTION();
    	
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Viewport");

        const auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        const auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        const auto viewportOffset = ImGui::GetWindowPos();
        m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
    	
        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);
    	
        const ImVec2 viewportSize = ImGui::GetContentRegionAvail();

        m_ViewportSize = { viewportSize.x, viewportSize.y };

        const uint32_t textureId =
            m_Framebuffer->GetColorAttachmentRendererID();

        ImGui::Image(
            reinterpret_cast<void*>(textureId),
            ImVec2(m_ViewportSize.x, m_ViewportSize.y),
            ImVec2(0, 1),
            ImVec2(1, 0)
        );

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
            {
                const wchar_t* path = (const wchar_t*)payload->Data;
                OpenScene(std::filesystem::path(g_AssetsPath) / path);
            }
            ImGui::EndDragDropTarget();
        }

    	//Gizmos
        Entity selectedEntity = m_Hierarchy.GetSelectedEntity();
    	if (selectedEntity && m_GizmoType != -1)
    	{
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();

            ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

            glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();
            const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();

            auto& tc = selectedEntity.GetComponent<TransformComponent>();
            glm::mat4 model = tc.GetModel();

            const bool snap = Input::IsKeyDown(KeyCode::LeftControl);
            const float snapValue = m_GizmoType == ImGuizmo::OPERATION::ROTATE ? 5.0f : 0.5f;

            float snapValues[3] = { snapValue, snapValue, snapValue };


            ImGuizmo::Manipulate(glm::value_ptr(cameraView),
                glm::value_ptr(cameraProjection),
                static_cast<ImGuizmo::OPERATION>(m_GizmoType),
                ImGuizmo::LOCAL,
                glm::value_ptr(model),
                nullptr,
                snap ? snapValues : nullptr);

    		if(ImGuizmo::IsUsing())
    		{
                glm::vec3 position, rotation, scale;
                Math::DecomposeTransform(model, position, rotation, scale);

                tc.position = position;

                //Adding delta rotation to avoid Gimbal lock
                tc.rotation += rotation - tc.rotation;

                tc.scale = scale;
    		}
    	}
    	
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void EditorLayer::DrawStatistics()
    {
        LEV_PROFILE_FUNCTION();
    	
        ImGui::Begin("Statistics");

        const auto stats = Renderer2D::GetStats();

        std::string hoveredEntity = "None";
        if (m_HoveredEntity)
            hoveredEntity = m_HoveredEntity.GetComponent<TagComponent>().tag;
    	
        ImGui::Text("Hovered Entity: %s", hoveredEntity.c_str());
        ImGui::Text("Draw Calls: %d", stats.drawCalls);
        ImGui::Text("Quads Count: %d", stats.quadCount);
        ImGui::Text("Vertices Count: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indices Count: %d", stats.GetTotalIndexCount());

        ImGui::End();
    }

    void EditorLayer::DrawDockSpace()
    {
        LEV_PROFILE_FUNCTION();
    	
        static bool open = true;
        static bool opt_fullscreen = true;

        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", &open, window_flags);
        ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        float minWinSizeX = style.WindowMinSize.x;
        style.WindowMinSize.x = 400.0f;
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        style.WindowMinSize.x = minWinSizeX;

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Scene", "Ctrl+N"))
                {
                    CreateNewScene();
                }

                if (ImGui::MenuItem("Open Scene...", "Ctrl+O"))
                {
                    OpenScene();
                }

                if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
                {
					SaveScene();
                }
            	
                if(ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
                {
                    SaveSceneAs();
                }
            	
                if (ImGui::MenuItem("Exit")) Application::Get().Close();

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::End();
    }
}
