#include "KatamariLayer.h"

#include "ObjLoader.h"
#include "../Kernel/Application.h"
#include "../GameObject.h"
#include "../Components/MeshRenderer.h"
#include "../OrbitCamera.h"
#include "../Components/SkyboxRenderer.h"
#include "../Assets.h"
#include "../Rigidbody.h"
#include "../Physics.h"
#include "KatamariPlayer.h"
#include "../Prefabs.h"
#include "../Random.h"
#define MAX_POINT_LIGHTS 10

struct alignas(16) CameraData
{
    Matrix ViewProjection;
    Vector3 Position;
};

struct DirLightData
{
    Vector4 Direction{Vector3(-DirectX::XM_PIDIV4 / 2, -DirectX::XM_PIDIV4, 0)};
    Vector4 Ambient{Vector3(0.3f, 0.3f, 0.3f)};
    Vector4 Diffuse{Vector3(0.7f, 0.6f, 0.1f)};
    Vector4 Specular{Vector3(1.0f, 1.0f, 1.0f)};
};

struct PointLightData
{
    alignas(16) Vector3 Position;

    float Constant = 1.0f;
    float Linear = 0.09f;
    float Quadratic = 0.032f;
    float _pad = 0;

    alignas(16) Vector3 Ambient{ 0.1f, 0.1f, 0.1f };
    alignas(16) Vector3 Diffuse{ 1.0f, 0.0f, 0.0f };
    alignas(16) Vector3 Specular{ 1.0f, 0.0f, 0.0f };
};

struct alignas(16) LightningData
{
    DirLightData DirLight;
    PointLightData PointLightsData[MAX_POINT_LIGHTS];
    int PointLightsCount = 0;
};

static std::vector<std::shared_ptr<GameObject>> gameObjects;

std::shared_ptr<GameObject> player;

void KatamariLayer::OnAttach()
{
    for (int i = 0; i < 20; i++)
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
    }

    auto floorCollider = std::make_shared<BoxCollider>(Vector3(150, 0.5f, 150));
    floorCollider->offset = Vector3::Down * 0.5f;
	auto floorGo = std::make_shared<GameObject>(
        std::make_shared<MeshRenderer>(ShaderAssets::Lit(), Mesh::CreatePlane(3), TextureAssets::Bricks(), 50),
        floorCollider);

    floorGo->GetTransform()->SetLocalScale(Vector3(300, 300, 1));
    floorGo->GetTransform()->SetWorldRotation(Vector3(90, 0, 0));
    floorGo->GetRigidbody()->bodyType = BodyType::Static;
    gameObjects.emplace_back(floorGo);

    m_Camera = std::make_shared<OrbitCamera>(45, 0.01f, 10000);
    m_Camera->SetPosition(Vector3(0, 50, 200));

    player = std::make_shared<KatamariPlayer>(m_Camera->GetTransform());
    player->GetTransform()->SetLocalPosition(Vector3::Up * 100);
    player->GetRigidbody()->gravityScale = 10;
    player->GetRigidbody()->angularDamping = 0.9f;
    player->GetRigidbody()->InitSphereInertia();
    gameObjects.emplace_back(player);

    m_Camera->SetTarget(player->GetTransform());

    m_CameraConstantBuffer = std::make_shared<D3D11ConstantBuffer>(sizeof CameraData);
    m_DirLightConstantBuffer = std::make_shared<D3D11ConstantBuffer>(sizeof LightningData, 2);
}

void KatamariLayer::OnUpdate(const float deltaTime)
{
    //Prepare Frame
    RenderCommand::Begin();
    auto& window = Application::Get().GetWindow();
    RenderCommand::SetViewport(0, 0, window.GetWidth(), window.GetHeight());
    m_Camera->SetViewportSize(window.GetWidth(), window.GetHeight());
    //window.DisableCursor();

    float color[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    RenderCommand::SetClearColor(color);
    RenderCommand::Clear();

    //Logic
    for (const auto& gameObject : gameObjects)
        gameObject->Update(deltaTime);

    UpdatePhysics(deltaTime, gameObjects);

    m_Camera->Update(deltaTime);

    //Draw
    for (const auto& gameObject : gameObjects)
        gameObject->GetTransform()->RecalculateModel();

    const CameraData cameraData{ m_Camera->GetViewProjection(), m_Camera->GetPosition() };
    m_CameraConstantBuffer->SetData(&cameraData, sizeof CameraData);

    auto playerLight = PointLightData{};
    playerLight.Position = player->GetTransform()->GetWorldPosition();

    const LightningData lightningData
    {
        DirLightData{},
        {playerLight},
    	1,
    };
    m_DirLightConstantBuffer->SetData(&lightningData, sizeof LightningData);

    for (const auto& gameObject : gameObjects)
	    gameObject->Draw();

    auto isOrtho = m_Camera->GetProjectionType() == SceneCamera::ProjectionType::Orthographic;

    if (isOrtho)
    {
        m_Camera->SetProjectionType(SceneCamera::ProjectionType::Perspective);
        const CameraData skyboxCameraData{ m_Camera->GetViewProjection(), Vector3::Zero };
        m_CameraConstantBuffer->SetData(&skyboxCameraData, sizeof CameraData);
        m_Camera->SetProjectionType(SceneCamera::ProjectionType::Orthographic);
    }

    static auto skybox = std::make_shared<SkyboxRenderer>(ShaderAssets::Skybox(), SkyboxAssets::Test());
    skybox->Draw(nullptr);

    //End frame
    RenderCommand::End();
}