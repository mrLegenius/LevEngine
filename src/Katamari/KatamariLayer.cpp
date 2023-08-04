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
#include "../Prefabs.h"
#include "Math/Random.h"
#include "Physics/Components/CollisionEvent.h"
#include "Physics/Events/CollisionEndEvent.h"
#include "Renderer/Material.h"

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

    otherTransform.SetParent(&myTransform);
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

    m_Scene = CreateRef<Scene>();

    for (int i = 0; i < 1; i++)
    {
        auto go = Prefabs::Gear(m_Scene);
        go.GetComponent<Transform>().SetWorldPosition(Vector3(Random::Float(-20, 20), 2, Random::Float(-20, 20)));

        auto& particles = go.AddComponent<EmitterComponent>();
        particles.Rate = 1;
        particles.Texture = TextureAssets::Particle();
        particles.MaxParticles = 10000;
        particles.Birth.GravityScale = 5;

        particles.Birth.RandomVelocity = true;
        particles.Birth.Velocity = Vector3{ -10, 20, -10 };
        particles.Birth.VelocityB = Vector3{ 10, 20, 10 };

        particles.Birth.RandomStartColor = true;
        particles.Birth.StartColor = Color{ 0.06, 0.37, 0.611, 1 };
        particles.Birth.StartColorB = Color{ 0.11, 0.64, 0.925, 1 };
        particles.Birth.EndColor = Color{ 0.83, 0.945, 0.976, 0 };

        particles.Birth.RandomStartSize = true;
        particles.Birth.StartSize = 0.2f;
        particles.Birth.StartSizeB = 0.5f;
        particles.Birth.EndSize = 0.1f;

        particles.Birth.RandomStartPosition = true;
        particles.Birth.Position = Vector3{ -1, 0, -1 };
        particles.Birth.PositionB = Vector3{ 1, 0, 1 };

        particles.Birth.RandomStartLifeTime = true;
        particles.Birth.LifeTime = 0.5;
        particles.Birth.LifeTimeB = 5;
    }

    for (int i = 0; i < 0; i++)
    {
	   	auto go = Prefabs::Log(m_Scene);
        go.GetComponent<Transform>().SetWorldPosition(Vector3(Random::Float(-100, 100), 1, Random::Float(-100, 100)));
    }

    for (int i = 0; i < 1; i++)
    {
        auto entity = m_Scene->CreateEntity("LavaRock");
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
            auto mesh = m_Scene->CreateEntity("LavaRockMesh");

            mesh.GetComponent<Transform>().SetParent(&entity.GetComponent<Transform>(), false);
            mesh.GetComponent<Transform>().SetLocalScale(Vector3::One * 0.2f);
            mesh.GetComponent<Transform>().SetLocalRotation(Vector3{ 90.0f, 0.0f, -90.0f });
            auto& meshRenderer = mesh.AddComponent<MeshRendererComponent>(ShaderAssets::Lit(), LavaRockAssets::Mesh());

            meshRenderer.material.SetAmbientColor(Color::White);
            meshRenderer.material.SetDiffuseColor(Color::White);
            meshRenderer.material.SetEmissiveColor(Color::White);
            meshRenderer.material.SetSpecularColor(Color::White);

            meshRenderer.material.SetTexture(Material::TextureType::Ambient, LavaRockAssets::AmbientTexture());
            meshRenderer.material.SetTexture(Material::TextureType::Diffuse, LavaRockAssets::AmbientTexture());
            meshRenderer.material.SetTexture(Material::TextureType::Emissive, LavaRockAssets::EmissiveTexture());
            meshRenderer.material.SetTexture(Material::TextureType::Specular, LavaRockAssets::SpecularTexture());
            meshRenderer.material.SetTexture(Material::TextureType::Normal, LavaRockAssets::NormalTexture());
        }

        //<--- Fire Particles ---<<
        {
            auto fireParticles = m_Scene->CreateEntity("LavaRock Fire Particles");
            fireParticles.GetComponent<Transform>().SetParent(&transform, false);
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
            auto smokeParticles = m_Scene->CreateEntity("LavaRock Smoke Particles");
            smokeParticles.GetComponent<Transform>().SetParent(&transform, false);
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

    for (int i = 0; i < 0; i++)
    {
        auto go = Prefabs::Sphere(m_Scene);
        auto& light = go.AddComponent<PointLightComponent>();
        const auto color = Vector3(
            Random::Float(0, 100) / 100.0f,
            Random::Float(0, 100) / 100.0f,
            Random::Float(0, 100) / 100.0f);

        light.Color = color;
        go.GetComponent<Transform>().SetWorldPosition(Vector3(Random::Float(-50, 50), 3, Random::Float(-50, 50)));
    }

    //<--- Floor ---<<
    auto floor = m_Scene->CreateEntity("Floor");
    auto& floorMesh = floor.AddComponent<MeshRendererComponent>(ShaderAssets::Lit(), Mesh::CreateCube());
    floorMesh.castShadow = false;
    floorMesh.material.SetTexture(Material::TextureType::Ambient, TextureAssets::Bricks());
    floorMesh.material.SetTexture(Material::TextureType::Diffuse, TextureAssets::Bricks());
    floorMesh.material.SetTextureTiling(Material::TextureType::Ambient, Vector2{16, 16});
    floorMesh.material.SetTextureTiling(Material::TextureType::Diffuse, Vector2{16, 16});

	floor.GetComponent<Transform>().SetLocalScale(Vector3(300, 1.0f, 300));
    floor.GetComponent<Transform>().SetWorldRotation(Vector3(0, 0, 0));
    auto& floorRigibody = floor.AddComponent<Rigidbody>();
    floorRigibody.bodyType = BodyType::Static;
    auto& floorCollider = floor.AddComponent<BoxCollider>();
    floorCollider.extents = Vector3(150, 0.5f, 150);

    //<--- Player ---<<
    auto player = m_Scene->CreateEntity("Player");
    auto& playerMesh = player.AddComponent<MeshRendererComponent>(ShaderAssets::Lit(), Mesh::CreateSphere(45));
    playerMesh.material = Materials::Ruby();

    auto& playerParticles = player.AddComponent<EmitterComponent>();
    playerParticles.Rate = 2;
    playerParticles.Texture = TextureAssets::Smoke();
    playerParticles.MaxParticles = 10000;
    playerParticles.Birth.Velocity = Vector3{ 0, 0, 0 };
    playerParticles.Birth.GravityScale = 0;

    playerParticles.Birth.RandomStartColor = true;
    playerParticles.Birth.StartColor = Color{ 1, 1, 1, 0.5 };
    playerParticles.Birth.StartColorB = Color{ 1, 1, 1, 1 };
    playerParticles.Birth.EndColor = Color{ 1, 1, 1, 0 };

    playerParticles.Birth.RandomStartSize = true;
    playerParticles.Birth.StartSize = 0.5f;
    playerParticles.Birth.StartSizeB = 0.75f;
    playerParticles.Birth.EndSize = 0.1f;

    playerParticles.Birth.RandomStartPosition = true;
    playerParticles.Birth.Position = Vector3{ -1, -1, -1 };
    playerParticles.Birth.PositionB = Vector3{ 1, 1, 1 };
   
    playerParticles.Birth.RandomStartLifeTime = true;
    playerParticles.Birth.LifeTime = 0.5;
    playerParticles.Birth.LifeTimeB = 1;

    auto& playerTransform = player.GetComponent<Transform>();
    playerTransform.SetLocalPosition(Vector3::One * 10);
    auto& playerRb = player.AddComponent<Rigidbody>();
    playerRb.gravityScale = 10;
    playerRb.angularDamping = 0.9f;
    playerRb.InitSphereInertia(playerTransform);
    player.AddComponent<SphereCollider>();
    auto& p = player.AddComponent<KatamariPlayerComponent>();
    auto& events = player.AddComponent<CollisionEvents>();
    events.onCollisionBegin.connect<&OnKatamariCollided>();
    auto& playerLight = player.AddComponent<PointLightComponent>();
    playerLight.Color = Vector3(1.0f, 0.0f, 0.0f);

    //<--- Camera ---<<
    auto camera = m_Scene->CreateEntity("Camera");
    camera.AddComponent<OrbitCamera>().SetTarget(playerTransform);
    camera.AddComponent<CameraComponent>();
    camera.GetComponent<Transform>().SetWorldPosition(Vector3{ 50, 100, 200 });
    camera.GetComponent<Transform>().SetWorldRotation(Vector3{ -30, 15, 0 });

    //<--- Skybox ---<<
    auto skybox = m_Scene->CreateEntity("Skybox");
    skybox.AddComponent<SkyboxRendererComponent>(SkyboxAssets::Interstellar());

    //<--- DirLight ---<<
    auto dirLight = m_Scene->CreateEntity("DirLight");
    auto& dirLightTransform = dirLight.GetComponent<Transform>();
    dirLightTransform.SetLocalRotation(Vector3(-45, 45, 0));
    dirLightTransform.SetWorldPosition(Vector3(150, 100.00f, 150)); 
    auto& dirLightComponent = dirLight.AddComponent<DirectionalLightComponent>();
    dirLightComponent.Color = Vector3{ 0.9f, 0.9f, 0.9f };

    auto& lightCamera = dirLight.AddComponent<CameraComponent>();
    lightCamera.camera.SetProjectionType(SceneCamera::ProjectionType::Orthographic);
    lightCamera.camera.SetOrthographic(0.25f, 100.0f, 1000.0f);
    lightCamera.isMain = true;

    //<--- Wall ---<<
    auto wall = m_Scene->CreateEntity("Wall");
    auto& wallMesh = wall.AddComponent<MeshRendererComponent>(ShaderAssets::Lit(), Mesh::CreateCube());
    wallMesh.material = Materials::BlackPlastic();
    auto& wallT = wall.GetComponent<Transform>();
    wallT.SetLocalScale(Vector3(1, 20, 100));
    wallT.SetWorldPosition(Vector3(150, 10, 50));

    //<--- Peak ---<<
    auto peak = m_Scene->CreateEntity("Peak");
    auto& peakMesh = peak.AddComponent<MeshRendererComponent>(ShaderAssets::Lit(), Mesh::CreateCube());
    peakMesh.material = Materials::Brass();
    auto& peakT = peak.GetComponent<Transform>();
    peakT.SetLocalScale(Vector3(10, 50, 10));
    peakT.SetWorldPosition(Vector3(100, 25, 100));

    //<--- Torch ---<<
    {
        auto entity = m_Scene->CreateEntity("Torch");
        auto mesh = entity.AddComponent<MeshRendererComponent>(ShaderAssets::Lit(), FancyTorch::Mesh());
        mesh.castShadow = false;
        mesh.material.SetTexture(Material::TextureType::Ambient, FancyTorch::AmbientTexture());
        mesh.material.SetTexture(Material::TextureType::Diffuse, FancyTorch::AmbientTexture());
        mesh.material.SetTexture(Material::TextureType::Specular, FancyTorch::SpecularTexture());
        mesh.material.SetTexture(Material::TextureType::Normal, FancyTorch::NormalTexture());

        entity.GetComponent<Transform>().SetWorldPosition(Vector3(10.f, 0.0f, 10.0f));
        entity.GetComponent<Transform>().SetLocalScale(Vector3(0.1f,0.1f, 0.1f));
        entity.GetComponent<Transform>().SetWorldRotation(Vector3(0, 0, 0));

        //<--- Fire Particles ---<<
        {
            auto fireParticles = m_Scene->CreateEntity("Torch Fire Particles");
            fireParticles.GetComponent<Transform>().SetParent(&entity.GetComponent<Transform>(), false);
            fireParticles.GetComponent<Transform>().SetLocalPosition(Vector3{ 0, 50, 0 });
            auto& particles = fireParticles.AddComponent<EmitterComponent>();
            particles.Rate = 1;
            particles.Texture = TextureAssets::Fire();
            particles.MaxParticles = 100;
            particles.Birth.Velocity = Vector3{ 0, 5, 0 };

            particles.Birth.StartColor = Color{ 1.0f, 0.7f, 0.0f, 1.0f };
            particles.Birth.EndColor = Color{ 0.5, 0, 0, 0 };

            particles.Birth.RandomStartSize = true;
            particles.Birth.StartSize = 0.2f;
            particles.Birth.StartSizeB = 0.5f;
            particles.Birth.EndSize = 0.1f;

            particles.Birth.RandomStartPosition = true;
            particles.Birth.Position = Vector3{ -0.5, -0.5, -0.5 };
            particles.Birth.PositionB = Vector3{ 0.5, 0.5, 0.5 };

            particles.Birth.RandomStartLifeTime = true;
            particles.Birth.LifeTime = 0.5;
            particles.Birth.LifeTimeB = 1;
        }

        //<--- Smoke Particles ---<<
        {
            auto smokeParticles = m_Scene->CreateEntity("Torch Smoke Particles");
            smokeParticles.GetComponent<Transform>().SetParent(&entity.GetComponent<Transform>(), false);
            smokeParticles.GetComponent<Transform>().SetLocalPosition(Vector3{ 0, 50, 0 });
            auto& particles = smokeParticles.AddComponent<EmitterComponent>();
            particles.Rate = 1;
            particles.Texture = TextureAssets::Smoke();
            particles.Birth.Velocity = Vector3{ 0, 6, 0 };

            particles.Birth.StartColor = Color{ 0xFFFFFFFF };
            particles.Birth.EndColor = Color{ 0xFFFFFF00 };

            particles.Birth.RandomStartSize = true;
            particles.Birth.StartSize = 0.1f;
            particles.Birth.StartSizeB = 0.3f;
            particles.Birth.EndSize = 0.05f;

            particles.Birth.RandomStartPosition = true;
            particles.Birth.Position = Vector3{ -0.3, -0.5, -0.3 };
            particles.Birth.PositionB = Vector3{ 0.3, 0.5, 0.3 };

            particles.Birth.RandomStartLifeTime = true;
            particles.Birth.LifeTime = 0.5;
            particles.Birth.LifeTimeB = 1;
        }
    }

    //<--- Systems ---<<
	m_Scene->RegisterLateUpdateSystem(CreateRef<OrbitCameraSystem>());
    m_Scene->RegisterLateUpdateSystem(CreateRef<KatamariCollisionSystem>());
    m_Scene->RegisterUpdateSystem(CreateRef<KatamariPlayerSystem>());
    m_Scene->RegisterUpdateSystem(CreateRef<TestSystem>());
    m_Scene->RegisterOneFrame<CollisionBeginEvent>();
    m_Scene->RegisterOneFrame<CollisionEndEvent>();
}

void KatamariLayer::OnEvent(Event& e)
{
	
}

static bool show_demo_window = true;

void KatamariLayer::OnGUIRender()
{
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    LEV_PROFILE_FUNCTION();

    DrawDockSpace();
    DrawViewport();
    //DrawStatistics();
    DrawToolbar();
    DrawStatusbar();
}

void KatamariLayer::OnUpdate(const float deltaTime)
{
    LEV_PROFILE_FUNCTION();

    //window.DisableCursor();

    m_Scene->OnUpdate(deltaTime);
    m_Scene->OnPhysics(deltaTime);
    m_Scene->OnLateUpdate(deltaTime);
    m_Scene->OnRender();
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
        /*if (m_SceneState == SceneState::Edit)
            OnScenePlay();
        else if (m_SceneState == SceneState::Play)
            OnSceneStop();*/
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
            {
                //CreateNewScene();
            }

            if (ImGui::MenuItem("Open Scene...", "Ctrl+O"))
            {
                //OpenScene();
            }

            if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
            {
                //SaveScene();
            }

            if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
            {
                //SaveSceneAs();
            }

            if (ImGui::MenuItem("Exit")) Application::Get().Close();

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::End();
}

void KatamariLayer::DrawViewport()
{
    LEV_PROFILE_FUNCTION();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");

    const auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
    const auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
    const auto viewportOffset = ImGui::GetWindowPos();
    m_ViewportBounds[0] = Vector2{ viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
    m_ViewportBounds[1] = Vector2{ viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

    m_ViewportFocused = ImGui::IsWindowFocused();
    m_ViewportHovered = ImGui::IsWindowHovered();
    Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

    const ImVec2 viewportSize = ImGui::GetContentRegionAvail();

    m_ViewportSize = Vector2{ viewportSize.x, viewportSize.y };

    const auto mainTexture = Application::Get().GetWindow().GetContext()->GetRenderTarget()->GetTexture(AttachmentPoint::Color0);

    ImGui::Image(
        mainTexture->GetId(),
        ImVec2(m_ViewportSize.x, m_ViewportSize.y),
        ImVec2(0, 1),
        ImVec2(1, 0)
    );

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
        {
            const wchar_t* path = (const wchar_t*)payload->Data;
            //OpenScene(std::filesystem::path(g_AssetsPath) / path);
        }
        ImGui::EndDragDropTarget();
    }

    //Gizmos
    //Entity selectedEntity = m_Hierarchy.GetSelectedEntity();
    //if (selectedEntity && m_GizmoType != -1)
    //{
    //    ImGuizmo::SetOrthographic(false);
    //    ImGuizmo::SetDrawlist();

    //    ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

    //    glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();
    //    const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();

    //    auto& tc = selectedEntity.GetComponent<TransformComponent>();
    //    glm::mat4 model = tc.GetModel();

    //    const bool snap = Input::IsKeyDown(KeyCode::LeftControl);
    //    const float snapValue = m_GizmoType == ImGuizmo::OPERATION::ROTATE ? 5.0f : 0.5f;

    //    float snapValues[3] = { snapValue, snapValue, snapValue };


    //    ImGuizmo::Manipulate(glm::value_ptr(cameraView),
    //        glm::value_ptr(cameraProjection),
    //        static_cast<ImGuizmo::OPERATION>(m_GizmoType),
    //        ImGuizmo::LOCAL,
    //        glm::value_ptr(model),
    //        nullptr,
    //        snap ? snapValues : nullptr);

    //    if (ImGuizmo::IsUsing())
    //    {
    //        glm::vec3 position, rotation, scale;
    //        Math::DecomposeTransform(model, position, rotation, scale);

    //        tc.position = position;

    //        //Adding delta rotation to avoid Gimbal lock
    //        tc.rotation += rotation - tc.rotation;

    //        tc.scale = scale;
    //    }
    //}

    ImGui::End();
    ImGui::PopStyleVar();
}