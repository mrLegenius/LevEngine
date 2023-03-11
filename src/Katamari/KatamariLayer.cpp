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

struct CameraData
{
    Matrix ViewProjection;
};


static std::vector<std::shared_ptr<GameObject>> gameObjects;

std::shared_ptr<GameObject> player;

void KatamariLayer::OnAttach()
{
	auto gear = std::make_shared<GameObject>(
        std::make_shared<MeshRenderer>(ShaderAssets::Unlit(), MeshAssets::Gear(), TextureAssets::Gear()),
        std::shared_ptr<Collider>(std::make_shared<SphereCollider>(5)));

    for(int i = 0; i < 10; i++)
    {
        auto logGo = std::make_shared<GameObject>(
            std::make_shared<MeshRenderer>(ShaderAssets::Unlit(), MeshAssets::Log(), TextureAssets::Log()),
            std::shared_ptr<Collider>(std::make_shared<SphereCollider>(2.5)));

        gameObjects.emplace_back(logGo);
        logGo->GetTransform()->SetWorldPosition(Vector3(rand() % 100, 10, rand() % 100));
        logGo->GetTransform()->SetLocalScale(Vector3::One * 5.0f);
        logGo->GetRigidbody()->gravityScale = 10;
        logGo->GetRigidbody()->mass = 0.5f;
    }

    /*auto log2 = CreateGameObject(MeshAssets::Log(), TextureAssets::Log(), shader);
    gameObjects.emplace_back(log2);
    log2->GetTransform()->SetPositionX(10);
    log2->GetTransform()->SetScale(DirectX::SimpleMath::Vector3::One * 2.0f);

    auto rock = CreateGameObject(MeshAssets::Rock(), TextureAssets::Rock(), shader);
    gameObjects.emplace_back(rock);
    rock->GetTransform()->SetScale(DirectX::SimpleMath::Vector3::One * 0.1f);*/

	auto floorGo = std::make_shared<GameObject>(
        std::make_shared<MeshRenderer>(ShaderAssets::Unlit(), Mesh::CreatePlane(3), TextureAssets::Bricks()),
        std::make_shared<BoxCollider>(Vector3(500, 0.5f, 500)));

    floorGo->GetTransform()->SetLocalScale(Vector3(1000, 1000, 1));
    floorGo->GetTransform()->SetWorldRotation(Vector3(90, 0, 0));
    floorGo->GetRigidbody()->bodyType = BodyType::Static;
    gameObjects.emplace_back(floorGo);

    m_Camera = std::make_shared<OrbitCamera>(45, 0.01f, 10000);
    m_Camera->SetPosition(Vector3(0, 50, 200));

    player = std::make_shared<KatamariPlayer>(m_Camera->GetTransform());
    player->GetTransform()->SetLocalPosition(Vector3::Up * 100);
    player->GetRigidbody()->gravityScale = 10;
    gameObjects.emplace_back(player);

    m_Camera->SetTarget(player->GetTransform());

    m_CameraConstantBuffer = std::make_shared<D3D11ConstantBuffer>(sizeof CameraData);
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
    for (auto gameObject : gameObjects)
        gameObject->Update(deltaTime);

    UpdatePhysics(deltaTime, gameObjects);

    m_Camera->Update(deltaTime);

    //Draw
    for (auto gameObject : gameObjects)
        gameObject->GetTransform()->RecalculateModel();

    const CameraData cameraData{ m_Camera->GetViewProjection() };
    m_CameraConstantBuffer->SetData(&cameraData, sizeof CameraData);

    for (const auto gameObject : gameObjects)
	    gameObject->Draw();

    auto isOrtho = m_Camera->GetProjectionType() == SceneCamera::ProjectionType::Orthographic;

    if (isOrtho)
    {
        m_Camera->SetProjectionType(SceneCamera::ProjectionType::Perspective);
        const CameraData skyboxCameraData{ m_Camera->GetViewProjection() };
        m_CameraConstantBuffer->SetData(&skyboxCameraData, sizeof CameraData);
        m_Camera->SetProjectionType(SceneCamera::ProjectionType::Orthographic);
    }

    static auto skybox = std::make_shared<SkyboxRenderer>(ShaderAssets::Skybox(), SkyboxAssets::Test());
    skybox->Draw(nullptr);

    //End frame
    RenderCommand::End();
}

void KatamariLayer::OnEvent(Event& event)
{
	
}