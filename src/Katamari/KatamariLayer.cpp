#include "KatamariLayer.h"

#include "ObjLoader.h"
#include "../Kernel/Application.h"
#include "../GameObject.h"
#include "../OrbitCamera.h"
#include "../Assets.h"
#include "../Rigidbody.h"
#include "../Physics.h"
#include "KatamariPlayer.h"
#include "../Prefabs.h"
#include "../Random.h"
#include "Scene/Entity.h"

static std::vector<std::shared_ptr<GameObject>> gameObjects;

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

  /*  auto player = m_Scene->CreateEntity("Player");
    player.AddComponent<MeshRenderer>(ShaderAssets::Lit(), Mesh::CreateSphere(45), TextureAssets::Rock());
    player.GetComponent<Transform>().SetLocalPosition(Vector3::Up * 100);*/

 //   auto floorCollider = std::make_shared<BoxCollider>(Vector3(150, 0.5f, 150));
 //   floorCollider->offset = Vector3::Down * 0.5f;
	//auto floorGo = std::make_shared<GameObject>(
 //       std::make_shared<MeshRenderer>(ShaderAssets::Lit(), Mesh::CreatePlane(3), TextureAssets::Bricks(), 50),
 //       floorCollider);

 //   floorGo->GetTransform()->SetLocalScale(Vector3(300, 300, 1));
 //   floorGo->GetTransform()->SetWorldRotation(Vector3(90, 0, 0));
 //   floorGo->GetRigidbody()->bodyType = BodyType::Static;
 //   gameObjects.emplace_back(floorGo);

    auto camera = m_Scene->CreateEntity("Camera");
    camera.AddComponent<OrbitCamera>();
    camera.AddComponent<CameraComponent>();

    /*player = std::make_shared<KatamariPlayer>(m_Camera->GetTransform());
    player->GetTransform()->SetLocalPosition(Vector3::Up * 100);
    player->GetRigidbody()->gravityScale = 10;
    player->GetRigidbody()->angularDamping = 0.9f;
    player->GetRigidbody()->InitSphereInertia();
    gameObjects.emplace_back(player);*/

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

void KatamariLayer::OnUpdate(const float deltaTime)
{
    for (const auto& gameObject : gameObjects)
        gameObject->Update(deltaTime);

    UpdatePhysics(deltaTime, gameObjects);

    //window.DisableCursor();
    
    float color[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    RenderCommand::SetClearColor(color);
    RenderCommand::Clear();

    //Draw
    m_Scene->OnUpdateRuntime(deltaTime);
    m_Scene->OnRenderRuntime();

    //End frame
}