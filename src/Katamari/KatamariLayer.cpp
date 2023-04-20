#include "KatamariLayer.h"

#include "ObjLoader.h"
#include "../Kernel/Application.h"
#include "../OrbitCamera.h"
#include "../Assets.h"
#include "Physics/Physics.h"
#include "KatamariPlayer.h"
#include "../Prefabs.h"
#include "../Random.h"
#include "Debugging/Profiler.h"
#include "Physics/Components/CollisionEvent.h"
#include "Physics/Events/CollisionEndEvent.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Material.h"

void OnKatamariCollided(Entity me, Entity other)
{
    auto& myCollider = me.GetComponent<SphereCollider>();
    auto& myTransform = me.GetComponent<Transform>();

    auto& otherRigidbody = other.GetComponent<Rigidbody>();
    auto& otherTransform = other.GetComponent<Transform>();
    if (otherRigidbody.bodyType == BodyType::Static) return;

    const auto size = myCollider.radius;
    const auto otherSize = LevEngine::Math::MaxElement(otherTransform.GetWorldScale());

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

            if (delta != 0)
            {
                std::cout << camera.camera.GetOrthographicSize() << std::endl;
            }
        }
	}
};
void KatamariLayer::OnAttach()
{
    LEV_PROFILE_FUNCTION();

    m_Scene = CreateRef<Scene>();

    for (int i = 0; i < 0; i++)
    {
        auto go = Prefabs::Gear(m_Scene);
        go.GetComponent<Transform>().SetWorldPosition(Vector3(Random::Range(-20, 20), 2, Random::Range(-20, 20)));
    }

    for (int i = 0; i < 0; i++)
    {
	   	auto go = Prefabs::Log(m_Scene);
        go.GetComponent<Transform>().SetWorldPosition(Vector3(Random::Range(-100, 100), 1, Random::Range(-100, 100)));
    }

    for (int i = 0; i < 3; i++)
    {
        auto go = Prefabs::LavaRock(m_Scene);
        go.GetComponent<Transform>().SetWorldPosition(Vector3(10 * i, 10, 0));

        auto& particles = go.AddComponent<EmitterComponent>();
        particles.Rate = 10;
        particles.Texture = TextureAssets::Fire();
        particles.MaxParticles = 10000;
        particles.Birth.Velocity = Vector3{ 0, 5, 0 };

        particles.Birth.StartColor = Color{ 1, 0.7, 0, 1 };
        particles.Birth.EndColor = Color{ 0.5, 0, 0, 0 };

        particles.Birth.RandomStartSize = true;
        particles.Birth.StartSize = 0.5f;
        particles.Birth.StartSizeB = 1.0f;
        particles.Birth.EndSize = 0.1f;

        particles.Birth.RandomStartPosition = true;
        particles.Birth.Position = Vector3{ -3.5, -0.5, -3 };
        particles.Birth.PositionB = Vector3{ 4, 0.5, 3 };

        particles.Birth.RandomStartLifeTime = true;
        particles.Birth.LifeTime = 0.5;
        particles.Birth.LifeTimeB = 1;
    }

    for (int i = 0; i < 0; i++)
    {
        auto go = Prefabs::Sphere(m_Scene);
        auto& light = go.AddComponent<PointLightComponent>();
        const auto color = Vector3(
            static_cast<float>(Random::Range(0, 100)) / 100.0f,
            static_cast<float>(Random::Range(0, 100)) / 100.0f, 
            static_cast<float>(Random::Range(0, 100)) / 100.0f);

        light.Color = color;
        go.GetComponent<Transform>().SetWorldPosition(Vector3(Random::Range(-50, 50), 3, Random::Range(-50, 50)));
    }

    //<--- Floor ---<<
    auto floor = m_Scene->CreateEntity("Floor");
    auto& floorMesh = floor.AddComponent<MeshRendererComponent>(ShaderAssets::Lit(), Mesh::CreateCube());
    floorMesh.castShadow = false;
    floorMesh.material = Materials::CyanPlastic();
    floorMesh.diffuseTexture = TextureAssets::Bricks();

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
    playerParticles.Rate = 10;
    playerParticles.Texture = TextureAssets::Smoke();
    playerParticles.MaxParticles = 10000;
    playerParticles.Birth.Velocity = Vector3{ 0, 5, 0 };

    playerParticles.Birth.StartColor = Color{ 1, 1, 1, 1 };
    playerParticles.Birth.EndColor = Color{ 1, 1, 1, 0 };

    playerParticles.Birth.RandomStartSize = true;
    playerParticles.Birth.StartSize = 1.0f;
    playerParticles.Birth.StartSizeB = 1.5f;
    playerParticles.Birth.EndSize = 0.1f;

    playerParticles.Birth.RandomStartPosition = true;
    playerParticles.Birth.Position = Vector3{ -1, 0, -1 };
    playerParticles.Birth.PositionB = Vector3{ 1, 0, 1 };
   
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
    skybox.AddComponent<SkyboxRendererComponent>(SkyboxAssets::Test());

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

void KatamariLayer::OnUpdate(const float deltaTime)
{
    LEV_PROFILE_FUNCTION();

    //window.DisableCursor();
    
    float color[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    RenderCommand::SetClearColor(color);
    RenderCommand::Clear();

    m_Scene->OnUpdate(deltaTime);
    m_Scene->OnPhysics(deltaTime);
    m_Scene->OnLateUpdate(deltaTime);
    m_Scene->OnRender();
}
