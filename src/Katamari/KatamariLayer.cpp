#include "KatamariLayer.h"

#include "ObjLoader.h"
#include "../Kernel/Application.h"
#include "../Pong/GameObject.h"
#include "../Components/MeshRenderer.h"
#include "../SolarSystem/OrbitCamera.h"
#include "../Components/SkyboxRenderer.h"
#include "../Assets.h"

struct CameraData
{
    DirectX::SimpleMath::Matrix ViewProjection;
};

std::vector<std::shared_ptr<GameObject>> objects;

std::shared_ptr<SkyboxRenderer> m_Skybox;

std::shared_ptr<GameObject> CreateGameObject(
    const std::shared_ptr<Mesh>& mesh,
    const std::shared_ptr<D3D11Texture2D>& texture,
    std::shared_ptr<D3D11Shader> shader)
{
    auto renderer = std::make_shared<MeshRenderer>(shader, mesh, texture, true);
    return std::make_shared<GameObject>(renderer);
}

void KatamariLayer::OnAttach()
{
    auto shader = ShaderAssets::Unlit();

    //auto gear = CreateGameObject(MeshAssets::Gear(), TextureAssets::Gear(), shader);
    //objects.emplace_back(gear);

    auto log = CreateGameObject(MeshAssets::Log(), TextureAssets::Log(), shader);
    objects.emplace_back(log);
    log->GetTransform()->SetPositionX(-10);
    log->GetTransform()->SetScale(DirectX::SimpleMath::Vector3::One * 2.0f);

    auto log2 = CreateGameObject(MeshAssets::Log(), TextureAssets::Log(), shader);
    objects.emplace_back(log2);
    log2->GetTransform()->SetPositionX(10);
    log2->GetTransform()->SetScale(DirectX::SimpleMath::Vector3::One * 2.0f);

    auto rock = CreateGameObject(MeshAssets::Rock(), TextureAssets::Rock(), shader);
    objects.emplace_back(rock);
    rock->GetTransform()->SetScale(DirectX::SimpleMath::Vector3::One * 0.1f);

    auto mesh = Mesh::CreatePlane(3);
    auto renderer = std::make_shared<MeshRenderer>(shader, mesh, TextureAssets::Bricks());
	auto floor = std::make_shared<GameObject>(renderer);
    floor->GetTransform()->SetScale(DirectX::SimpleMath::Vector3(1000, 1000, 1));
    floor->GetTransform()->SetRotation(DirectX::SimpleMath::Vector3(90, 0, 0));
    objects.emplace_back(floor);

    m_Skybox = std::make_shared<SkyboxRenderer>(ShaderAssets::Skybox(), SkyboxAssets::Test());

    m_Camera = std::make_shared<FreeCamera>(45, 0.01f, 10000);
    m_Camera->SetPosition(DirectX::SimpleMath::Vector3(0, 50, 200));

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

    const CameraData cameraData{ m_Camera->GetViewProjection() };
    m_CameraConstantBuffer->SetData(&cameraData, sizeof CameraData);

    //Logic
    m_Camera->Update(deltaTime);

    //Draw
    for (const auto gameObject : objects)
	    gameObject->Draw(); 

    auto isOrtho = m_Camera->GetProjectionType() == SceneCamera::ProjectionType::Orthographic;

    if (isOrtho)
    {
        m_Camera->SetProjectionType(SceneCamera::ProjectionType::Perspective);
        const CameraData skyboxCameraData{ m_Camera->GetViewProjection() };
        m_CameraConstantBuffer->SetData(&skyboxCameraData, sizeof CameraData);
        m_Camera->SetProjectionType(SceneCamera::ProjectionType::Orthographic);
    }
    m_Skybox->Draw(nullptr);

    //End frame
    RenderCommand::End();
}

void KatamariLayer::OnEvent(Event& event)
{
	
}
