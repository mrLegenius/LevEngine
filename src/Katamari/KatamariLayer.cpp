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
#include "../Input/Input.h"
#include "../Input/KeyCodes.h"

struct CameraData
{
    Matrix ViewProjection;
};

static std::vector<std::shared_ptr<GameObject>> gameObjects;

std::shared_ptr<GameObject> floorGo;
std::shared_ptr<GameObject> logGo;
std::shared_ptr<GameObject> player;

std::shared_ptr<GameObject> CreateGameObject(
    const std::shared_ptr<Mesh>& mesh,
    const std::shared_ptr<D3D11Texture2D>& texture,
    std::shared_ptr<D3D11Shader> shader)
{
    auto renderer = std::make_shared<MeshRenderer>(shader, mesh, texture);
    return std::make_shared<GameObject>(renderer);
}

std::shared_ptr<GameObject> CreateGameObject(
    const std::shared_ptr<Mesh>& mesh,
    const std::shared_ptr<D3D11Texture2D>& texture,
    std::shared_ptr<D3D11Shader> shader,
    std::shared_ptr<Collider> collider)
{
    auto renderer = std::make_shared<MeshRenderer>(shader, mesh, texture);
    return std::make_shared<GameObject>(renderer, collider);
}

void KatamariLayer::OnAttach()
{
	/*auto gear = CreateGameObject(MeshAssets::Gear(), TextureAssets::Gear(), ShaderAssets::Unlit());
    gear->GetRigidbody()->velocity = DirectX::SimpleMath::Vector3(1, 0, 0);
    gear->GetRigidbody()->gravityScale = 0;
    gameObjects.emplace_back(gear);*/

    auto logCollider = std::make_shared<SphereCollider>();
    logCollider->radius = 5;
    logGo = CreateGameObject(MeshAssets::Log(), TextureAssets::Log(), ShaderAssets::Unlit(), logCollider);
    //gameObjects.emplace_back(logGo);
    logGo->GetTransform()->SetPositionY(50);
    logGo->GetTransform()->SetPositionX(10);
    logGo->GetTransform()->SetScale(Vector3::One * 10.0f);
    logGo->GetRigidbody()->gravityScale = 10;
    logGo->GetRigidbody()->mass = 10.0f;

    /*auto log2 = CreateGameObject(MeshAssets::Log(), TextureAssets::Log(), shader);
    gameObjects.emplace_back(log2);
    log2->GetTransform()->SetPositionX(10);
    log2->GetTransform()->SetScale(DirectX::SimpleMath::Vector3::One * 2.0f);

    auto rock = CreateGameObject(MeshAssets::Rock(), TextureAssets::Rock(), shader);
    gameObjects.emplace_back(rock);
    rock->GetTransform()->SetScale(DirectX::SimpleMath::Vector3::One * 0.1f);*/

	floorGo = std::make_shared<GameObject>(
        std::make_shared<MeshRenderer>(ShaderAssets::Unlit(), Mesh::CreatePlane(3), TextureAssets::Bricks()),
        std::make_shared<BoxCollider>(Vector3(500, 0.5f, 500)));

    floorGo->GetTransform()->SetScale(Vector3(1000, 1000, 1));
    floorGo->GetTransform()->SetRotation(Vector3(90, 0, 0));
    floorGo->GetRigidbody()->bodyType = BodyType::Static;
    gameObjects.emplace_back(floorGo);

	player = CreateGameObject(Mesh::CreateSphere(45), TextureAssets::Rock(), ShaderAssets::Unlit(), std::make_shared<SphereCollider>());
    player->GetTransform()->SetPosition(Vector3::Up * 100);
    player->GetRigidbody()->gravityScale = 10;
    gameObjects.emplace_back(player);

    m_Camera = std::make_shared<OrbitCamera>(45, 0.01f, 10000, player->GetTransform());
    m_Camera->SetPosition(Vector3(0, 50, 200));

    m_CameraConstantBuffer = std::make_shared<D3D11ConstantBuffer>(sizeof CameraData);
}

void KatamariLayer::OnUpdate(const float deltaTime)
{
    //Prepare Frame
    RenderCommand::Begin();
    auto& window = Application::Get().GetWindow();
    RenderCommand::SetViewport(0, 0, window.GetWidth(), window.GetHeight());
    m_Camera->SetViewportSize(window.GetWidth(), window.GetHeight());
    window.DisableCursor();

    float color[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    RenderCommand::SetClearColor(color);
    RenderCommand::Clear();

    //Logic
    if (Input::IsKeyPressed(KeyCode::Space))
        player->GetRigidbody()->AddImpulse(Vector3::Up * 100);

    Vector3 movementDir = Vector3::Zero;

    if (Input::IsKeyDown(KeyCode::W))
        movementDir = m_Camera->GetTransform().GetForwardDirection();
    if (Input::IsKeyDown(KeyCode::A))
        movementDir = -m_Camera->GetTransform().GetRightDirection();
    if (Input::IsKeyDown(KeyCode::S))
        movementDir = -m_Camera->GetTransform().GetForwardDirection();
    if (Input::IsKeyDown(KeyCode::D))
        movementDir = m_Camera->GetTransform().GetRightDirection();

    movementDir.y = 0.0f;
    movementDir.Normalize();
    player->GetRigidbody()->AddForceAtPosition(movementDir * 100, player->GetTransform()->GetPosition() + Vector3::Up);

    UpdatePhysics(deltaTime, gameObjects);

    m_Camera->Update(deltaTime);

    //Draw
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