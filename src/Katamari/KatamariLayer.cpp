#include "KatamariLayer.h"

#include "ObjLoader.h"
#include "../Kernel/Application.h"
#include "../OrbitCamera.h"
#include "../Assets.h"
#include "Physics/Physics.h"
#include "KatamariPlayer.h"
#include "../Prefabs.h"
#include "../Random.h"
#include "Renderer/RenderCommand.h"
#include "Scene/Entity.h"

void KatamariLayer::OnAttach()
{
    m_Scene = CreateRef<Scene>();

    /*for (int i = 0; i < 20; i++)
    {
        auto go = Prefabs::Gear(gameObjects);
        go->GetTransform()->SetWorldPosition(Vector3(Random::Range(-20, 20), 2, Random::Range(-20, 20)));
    }

    for (int i = 0; i < 10; i++)
    {
	   	auto go = Prefabs::Log(gameObjects);
        go->GetTransform()->SetWorldPosition(Vector3(Random::Range(-100, 100), 1, Random::Range(-100, 100)));
    }

    for (int i = 0; i < 10; i++)
    {
        auto go = Prefabs::Rock(gameObjects);
        go->GetTransform()->SetWorldPosition(Vector3(Random::Range(-100, 100), 1, Random::Range(-100, 100)));
    }*/

    auto floor = m_Scene->CreateEntity("Floor");
    floor.AddComponent<MeshRendererComponent>(ShaderAssets::Lit(), Mesh::CreatePlane(3), TextureAssets::Bricks());
	floor.GetComponent<Transform>().SetLocalScale(Vector3(300, 300, 1));
    floor.GetComponent<Transform>().SetWorldRotation(Vector3(90, 0, 0));
    auto& floorRigibody = floor.AddComponent<Rigidbody>();
    floorRigibody.bodyType = BodyType::Static;
    auto& floorCollider = floor.AddComponent<BoxCollider>();
    floorCollider.extents = Vector3(150, 0.5f, 150);
    floorCollider.offset = Vector3::Down * 0.5f;

    auto player = m_Scene->CreateEntity("Player");
    player.AddComponent<MeshRendererComponent>(ShaderAssets::Lit(), Mesh::CreateSphere(45), TextureAssets::Rock());
    auto& playerTransform = player.GetComponent<Transform>();
    playerTransform.SetLocalPosition(Vector3::Up * 100);
    auto& playerRb = player.AddComponent<Rigidbody>();
    playerRb.gravityScale = 10;
    playerRb.angularDamping = 0.9f;
    playerRb.InitSphereInertia(playerTransform);
    player.AddComponent<SphereCollider>();

    auto camera = m_Scene->CreateEntity("Camera");
    camera.AddComponent<OrbitCamera>().SetTarget(playerTransform);
    camera.AddComponent<CameraComponent>();

    //m_Camera->SetTarget(&player.GetComponent<Transform>());
    //m_CameraConstantBuffer = std::make_shared<D3D11ConstantBuffer>(sizeof CameraData);
    //m_DirLightConstantBuffer = std::make_shared<D3D11ConstantBuffer>(sizeof LightningData, 2);
    auto skybox = m_Scene->CreateEntity("Skybox");
    skybox.AddComponent<SkyboxRendererComponent>(SkyboxAssets::Test());

    auto dirLight = m_Scene->CreateEntity("DirLight");
    auto& dirLightTransform = dirLight.GetComponent<Transform>();
    dirLightTransform.SetLocalRotation(Vector3(45, -90, 0));
    auto& dirLightComponent = dirLight.AddComponent<DirectionalLightComponent>();
    dirLightComponent.Ambient = Vector3{ 0.3f, 0.3f, 0.3f };
    dirLightComponent.Diffuse = Vector3{ 1.0f, 1.0f, 1.0f };
    dirLightComponent.Specular = Vector3{ 1.0f, 1.0f, 1.0f };
}

void KatamariLayer::OnEvent(Event& e)
{
	
}

void KatamariLayer::OnUpdate(const float deltaTime)
{
    //window.DisableCursor();
    
    float color[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    RenderCommand::SetClearColor(color);
    RenderCommand::Clear();

    m_Scene->OnUpdate(deltaTime);
    m_Scene->OnPhysics(deltaTime);
    m_Scene->OnRender();
}