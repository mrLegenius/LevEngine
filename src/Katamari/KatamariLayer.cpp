#include "pch.h"
#include "KatamariLayer.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "ObjLoader.h"
#include "../Kernel/Application.h"
#include "../OrbitCamera.h"
#include "../Assets.h"
#include "Physics/Physics.h"
#include "KatamariPlayer.h"
#include "Kernel/PlatformUtils.h"
#include "Math/Random.h"
#include "Physics/Components/CollisionEvent.h"
#include "Physics/Events/CollisionEndEvent.h"
#include "Renderer/Material.h"
#include "Renderer/Renderer.h"
#include "Scene/Components/Camera/Camera.h"
#include "Scene/Serializers/SceneSerializer.h"

void OnKatamariCollided(Entity me, Entity other)
{
    auto& myCollider = me.GetComponent<SphereCollider>();
    auto& myTransform = me.GetComponent<Transform>();

    auto& otherRigidbody = other.GetComponent<Rigidbody>();
    auto& otherTransform = other.GetComponent<Transform>();
    if (otherRigidbody.bodyType == BodyType::Static) return;

    const auto size = myCollider.radius;
    const auto otherSize = Math::MaxElement(otherTransform.GetWorldScale());

    if (size <= otherSize) return;

    myCollider.radius += otherSize;

    otherRigidbody.enabled = false;

    otherTransform.SetParent(me);
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
void KatamariLayer::OnAttach()
{
    LEV_PROFILE_FUNCTION();

    m_IconPlay = Texture::Create("resources/Icons/PlayButton.png");
    m_IconStop = Texture::Create("resources/Icons/StopButton.png");

    m_ActiveScene = CreateRef<Scene>();

    m_Viewport = CreateRef<Editor::ViewportPanel>(Application::Get().GetWindow().GetContext()->GetRenderTarget()->GetTexture(AttachmentPoint::Color0));
    m_Hierarchy = CreateRef<Editor::HierarchyPanel>(m_ActiveScene);
    m_Properties = CreateRef<Editor::PropertiesPanel>();
    m_AssetsBrowser = CreateRef<Editor::AssetBrowserPanel>();

    for (int i = 0; i < 1; i++)
    {
        auto entity = m_ActiveScene->CreateEntity("LavaRock");
        auto& transform = entity.GetComponent<Transform>();

        //<--- Transform ---<<
        {
            transform.SetLocalScale(Vector3::One);
            transform.SetWorldPosition(Vector3(10 * i, 20, 0));
        }

        //<--- Physics ---<<
        {
            constexpr auto colliderScale = 2.0f;
            entity.AddComponent<BoxCollider>(Vector3(colliderScale, colliderScale, colliderScale));
            auto& rb = entity.AddComponent<Rigidbody>();

            rb.gravityScale = 2;
            rb.mass = 100.0f;
            rb.InitCubeInertia(transform);
        }

        //<--- Mesh ---<<
        {
            auto mesh = m_ActiveScene->CreateEntity("LavaRockMesh");

            mesh.GetComponent<Transform>().SetParent(entity, false);
            mesh.GetComponent<Transform>().SetLocalScale(Vector3::One * 0.2f);
            mesh.GetComponent<Transform>().SetLocalRotation(Vector3{ 90.0f, 0.0f, -90.0f });
            /*auto& meshRenderer = mesh.AddComponent<MeshRendererComponent>(LavaRockAssets::Mesh());
            meshRenderer.material = CreateRef<MaterialAsset>("");
            meshRenderer.material->material.SetAmbientColor(Color::White);
            meshRenderer.material->material.SetDiffuseColor(Color::White);
            meshRenderer.material->material.SetEmissiveColor(Color::White);
            meshRenderer.material->material.SetSpecularColor(Color::White);

            meshRenderer.material->material.SetTexture(Material::TextureType::Ambient, LavaRockAssets::AmbientTexture());
            meshRenderer.material->material.SetTexture(Material::TextureType::Diffuse, LavaRockAssets::AmbientTexture());
            meshRenderer.material->material.SetTexture(Material::TextureType::Emissive, LavaRockAssets::EmissiveTexture());
            meshRenderer.material->material.SetTexture(Material::TextureType::Specular, LavaRockAssets::SpecularTexture());
            meshRenderer.material->material.SetTexture(Material::TextureType::Normal, LavaRockAssets::NormalTexture());*/
        }

        //<--- Fire Particles ---<<
        {
            auto fireParticles = m_ActiveScene->CreateEntity("LavaRock Fire Particles");
            fireParticles.GetComponent<Transform>().SetParent(entity, false);
            fireParticles.GetComponent<Transform>().SetLocalPosition(Vector3{ 0, 0, 0 });
            auto& particles = fireParticles.AddComponent<EmitterComponent>();
            particles.Rate = 5;
            particles.Texture = TextureAssets::Fire();
            particles.MaxParticles = 10000;
            particles.Birth.Velocity = Vector3{ 0, 5, 0 };

            particles.Birth.StartColor = Color{ 1.0f, 0.7f, 0.0f, 1.0f };
            particles.Birth.EndColor = Color{ 0.5, 0, 0, 0 };

            particles.Birth.RandomStartSize = true;
            particles.Birth.StartSize = 0.5f;
            particles.Birth.StartSizeB = 1.0f;
            particles.Birth.EndSize = 0.1f;

            particles.Birth.RandomStartPosition = true;
            particles.Birth.Position = Vector3{ -3, -0.5, -2.5 };
            particles.Birth.PositionB = Vector3{ 3, 0.5, 2.5 };

            particles.Birth.RandomStartLifeTime = true;
            particles.Birth.LifeTime = 0.5;
            particles.Birth.LifeTimeB = 1;
        }

        //<--- Smoke Particles ---<<
        {
            auto smokeParticles = m_ActiveScene->CreateEntity("LavaRock Smoke Particles");
            smokeParticles.GetComponent<Transform>().SetParent(entity, false);
            auto& particles = smokeParticles.AddComponent<EmitterComponent>();
            particles.Rate = 1;
            particles.Texture = TextureAssets::Smoke();
            particles.Birth.Velocity = Vector3{ 0, 10, 0 };

            particles.Birth.StartColor = Color{ 0xFFFFFFFF };
            particles.Birth.EndColor = Color{ 0xFFFFFF00 };

            particles.Birth.RandomStartSize = true;
            particles.Birth.StartSize = 0.5f;
            particles.Birth.StartSizeB = 1.0f;
            particles.Birth.EndSize = 0.1f;

            particles.Birth.RandomStartPosition = true;
            particles.Birth.Position = Vector3{ -2, -0.5, -2 };
            particles.Birth.PositionB = Vector3{ 2, 0.5, 2 };

            particles.Birth.RandomStartLifeTime = true;
            particles.Birth.LifeTime = 0.5;
            particles.Birth.LifeTimeB = 1;
        }
        
    }

    ////<--- Player ---<<
    //auto player = m_ActiveScene->CreateEntity("Player");
    //auto& playerMesh = player.AddComponent<MeshRendererComponent>(Mesh::CreateSphere(45));
    //playerMesh.material = CreateRef<MaterialAsset>("");
    //playerMesh.material->material = Materials::Ruby();

    //auto& playerParticles = player.AddComponent<EmitterComponent>();
    //playerParticles.Rate = 2;
    //playerParticles.Texture = TextureAssets::Smoke();
    //playerParticles.MaxParticles = 10000;
    //playerParticles.Birth.Velocity = Vector3{ 0, 0, 0 };
    //playerParticles.Birth.GravityScale = 0;

    //playerParticles.Birth.RandomStartColor = true;
    //playerParticles.Birth.StartColor = Color{ 1, 1, 1, 0.5 };
    //playerParticles.Birth.StartColorB = Color{ 1, 1, 1, 1 };
    //playerParticles.Birth.EndColor = Color{ 1, 1, 1, 0 };

    //playerParticles.Birth.RandomStartSize = true;
    //playerParticles.Birth.StartSize = 0.5f;
    //playerParticles.Birth.StartSizeB = 0.75f;
    //playerParticles.Birth.EndSize = 0.1f;

    //playerParticles.Birth.RandomStartPosition = true;
    //playerParticles.Birth.Position = Vector3{ -1, -1, -1 };
    //playerParticles.Birth.PositionB = Vector3{ 1, 1, 1 };
   
    //playerParticles.Birth.RandomStartLifeTime = true;
    //playerParticles.Birth.LifeTime = 0.5;
    //playerParticles.Birth.LifeTimeB = 1;

    //auto& playerTransform = player.GetComponent<Transform>();
    //playerTransform.SetLocalPosition(Vector3::One * 10);
    //auto& playerRb = player.AddComponent<Rigidbody>();
    //playerRb.gravityScale = 10;
    //playerRb.angularDamping = 0.9f;
    //playerRb.InitSphereInertia(playerTransform);
    //player.AddComponent<SphereCollider>();
    //auto& p = player.AddComponent<KatamariPlayerComponent>();
    //auto& events = player.AddComponent<CollisionEvents>();
    //events.onCollisionBegin.connect<&OnKatamariCollided>();
    //auto& playerLight = player.AddComponent<PointLightComponent>();
    //playerLight.color = Color(1.0f, 0.0f, 0.0f);

    ////<--- Camera ---<<
    //auto camera = m_ActiveScene->CreateEntity("Camera");
    //camera.AddComponent<OrbitCamera>().SetTarget(playerTransform);
    //camera.AddComponent<CameraComponent>();
    //camera.GetComponent<Transform>().SetWorldPosition(Vector3{ 50, 100, 200 });
    //camera.GetComponent<Transform>().SetWorldRotation(Vector3{ -30, 15, 0 });

    ////<--- Skybox ---<<
    //auto skybox = m_ActiveScene->CreateEntity("Skybox");
    //skybox.AddComponent<SkyboxRendererComponent>();

    ////<--- DirLight ---<<
    //auto dirLight = m_ActiveScene->CreateEntity("DirLight");
    //auto& dirLightTransform = dirLight.GetComponent<Transform>();
    //dirLightTransform.SetLocalRotation(Vector3(-45, 45, 0));
    //dirLightTransform.SetWorldPosition(Vector3(150, 100.00f, 150));
    //auto& dirLightComponent = dirLight.AddComponent<DirectionalLightComponent>();
    //dirLightComponent.color = Color{ 0.9f, 0.9f, 0.9f };

    //auto& lightCamera = dirLight.AddComponent<CameraComponent>();
    //lightCamera.camera.SetProjectionType(SceneCamera::ProjectionType::Orthographic);
    //lightCamera.camera.SetOrthographic(0.25f, 100.0f, 1000.0f);
    //lightCamera.isMain = false;

    //<--- Systems ---<<
	m_ActiveScene->RegisterLateUpdateSystem(CreateRef<OrbitCameraSystem>());
    m_ActiveScene->RegisterLateUpdateSystem(CreateRef<KatamariCollisionSystem>());
    m_ActiveScene->RegisterUpdateSystem(CreateRef<KatamariPlayerSystem>());
    m_ActiveScene->RegisterUpdateSystem(CreateRef<TestSystem>());
    m_ActiveScene->RegisterOneFrame<CollisionBeginEvent>();
    m_ActiveScene->RegisterOneFrame<CollisionEndEvent>();

    Application::Get().GetWindow().EnableCursor();
}

void KatamariLayer::OnEvent(Event& event)
{
    EventDispatcher dispatcher{ event };
    dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(KatamariLayer::OnKeyPressed));
    dispatcher.Dispatch<WindowResizedEvent>(BIND_EVENT_FN(KatamariLayer::OnWindowResized));
}

bool KatamariLayer::OnKeyPressed(KeyPressedEvent& event)
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

bool KatamariLayer::OnWindowResized(const WindowResizedEvent& e) const
{
    m_ActiveScene->OnViewportResized(e.GetWidth(), e.GetHeight());
    Renderer::SetViewport(e.GetWidth(), e.GetHeight());
    return false;
}

void KatamariLayer::OnUpdate(const float deltaTime)
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
        m_Viewport->UpdateCamera(deltaTime);
        auto& camera = m_Viewport->GetCamera();
        m_ActiveScene->OnRender(&camera, &camera.GetTransform());
        break;
    }
    case SceneState::Play:
    {
        if (m_Viewport->IsFocused())
	        Application::Get().GetWindow().DisableCursor();

        m_ActiveScene->OnUpdate(deltaTime);
        m_ActiveScene->OnPhysics(deltaTime);
        m_ActiveScene->OnLateUpdate(deltaTime);
        m_ActiveScene->OnRender();
        break;
    }
    }

    m_Viewport->UpdateViewportTexture(Application::Get().GetWindow().GetContext()->GetRenderTarget()->GetTexture(AttachmentPoint::Color0));
}

void KatamariLayer::OnGUIRender()
{
	ImGui::ShowDemoWindow(nullptr);

    LEV_PROFILE_FUNCTION();

    DrawDockSpace();
    m_Viewport->Render();
    m_Hierarchy->Render();
    m_Properties->Render();
    m_AssetsBrowser->Render();
    //DrawStatistics();
    DrawToolbar();
    DrawStatusbar();
}

constexpr float toolbarSize = 10;
float menuBarHeight;

void KatamariLayer::DrawToolbar()
{
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

    float size = ImGui::GetWindowHeight() - 4.0f;
    Ref<Texture> icon = m_SceneState == SceneState::Edit ? m_IconPlay : m_IconStop;
    
    ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
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

void KatamariLayer::DrawStatusbar()
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

void KatamariLayer::DrawDockSpace()
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

void KatamariLayer::CreateNewScene()
{
    m_ActiveScene = CreateRef<Scene>();
    /*m_ActiveScene->OnViewportResized(
        static_cast<uint32_t>(m_Viewport->GetWidth()),
        static_cast<uint32_t>(m_Viewport->GetHeight()));*/
    m_Hierarchy->SetContext(m_ActiveScene);
    m_EditorScenePath = std::filesystem::path();
}

void KatamariLayer::OpenScene()
{
    const auto filepath = FileDialogs::OpenFile("LevEngine Scene (*.scene)\0*.scene\0");
    if (!filepath.empty())
    {
        OpenScene(filepath);
    }
}

void KatamariLayer::OpenScene(const std::filesystem::path& path)
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
        m_ActiveScene->RegisterLateUpdateSystem(CreateRef<OrbitCameraSystem>());
        m_ActiveScene->RegisterLateUpdateSystem(CreateRef<KatamariCollisionSystem>());
        m_ActiveScene->RegisterUpdateSystem(CreateRef<KatamariPlayerSystem>());
        m_ActiveScene->RegisterUpdateSystem(CreateRef<TestSystem>());
        m_ActiveScene->RegisterOneFrame<CollisionBeginEvent>();
        m_ActiveScene->RegisterOneFrame<CollisionEndEvent>();
    }
}

bool KatamariLayer::SaveScene()
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

bool KatamariLayer::SaveSceneAs()
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

void KatamariLayer::OnScenePlay()
{
    if (!SaveScene()) return;

    m_Viewport->Focus();
    m_SceneState = SceneState::Play;
    Selection::Deselect();
}

void KatamariLayer::OnSceneStop()
{
    m_SceneState = SceneState::Edit;
    Selection::Deselect();

    OpenScene(m_EditorScenePath);
}