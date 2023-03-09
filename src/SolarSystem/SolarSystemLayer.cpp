#include "SolarSystemLayer.h"

#include "CircularMovement.h"
#include "../Renderer/3D/Mesh.h"
#include "../Renderer/RenderCommand.h"
#include "../Renderer/D3D11Texture.h"
#include "../Renderer/D3D11Shader.h"
#include "../Kernel/Application.h"
#include "../Input/Input.h"

#include "../GameObject.h"
#include "Body.h"
#include "../Components/SkyboxRenderer.h"
#include "../Assets.h"

void DrawMesh(const DirectX::SimpleMath::Matrix& transform, const Mesh& mesh, const std::shared_ptr<D3D11Shader>& shader, const std::shared_ptr<D3D11Texture2D>& texture);

struct CameraData
{
    Matrix ViewProjection;
};

float orbitRadiusScale = 3;

BodyParameters Sun{ 69.634f, 0, 0, 0, 20.0f,"./resources/Textures/sun.jpg" };
BodyParameters Mercury{ 2.44f, 35 * orbitRadiusScale, 0.479f, 0,0.11f,"./resources/Textures/mercury.jpg" };
BodyParameters Venus{ 6.05f, 67 * orbitRadiusScale, 0.35f, 0,0.07f,"./resources/Textures/venus.jpg" };
BodyParameters Earth{ 6.37f, 93 * orbitRadiusScale, 29.8f / 5, 0, 15.74f,"./resources/Textures/earth.jpg" };
BodyParameters Moon{ 1.7f, 38.4f, 1, 0,15.74f,"./resources/Textures/moon.jpg" };
BodyParameters Mars{ 3.389f, 142 * orbitRadiusScale, 0.241f, 0,8.6f,"./resources/Textures/mars.jpg" };
BodyParameters Jupiter{ 69.91f, 484 * orbitRadiusScale, .131f, 0,455.83f,"./resources/Textures/jupiter.jpg" };
BodyParameters Saturn{ 58.23f, 889 * orbitRadiusScale, 0.097f, 0,36.840f,"./resources/Textures/saturn.jpg" };
BodyParameters Uranus{ 25.36f, 1790 * orbitRadiusScale, 0.068f, 0,147.94f,"./resources/Textures/uranus.jpg" };
BodyParameters Neptune{ 24.62f, 2880 * orbitRadiusScale, 0.054f, 0,97.19f,"./resources/Textures/neptune.jpg" };

void SolarSystemLayer::OnAttach()
{
    auto shader = ShaderAssets::Unlit();

    m_Skybox = std::make_shared<SkyboxRenderer>(ShaderAssets::Skybox(), SkyboxAssets::LightBlue());

    m_CameraConstantBuffer = std::make_shared<D3D11ConstantBuffer>(sizeof CameraData);

    m_Sun = std::make_shared<Body>(nullptr, Sun, shader);
    m_Mercury = std::make_shared<Body>(m_Sun, Mercury, shader);
    m_Venus = std::make_shared<Body>(m_Sun, Venus, shader);
    m_Earth = std::make_shared<Body>(m_Sun, Earth, shader);
    m_Moon = std::make_shared<Body>(m_Earth, Moon, shader);
    m_Mars = std::make_shared<Body>(m_Sun, Mars, shader);
    m_Jupiter = std::make_shared<Body>(m_Sun, Jupiter, shader);
    m_Saturn = std::make_shared<Body>(m_Sun, Saturn, shader);
    m_Uranus = std::make_shared<Body>(m_Sun, Uranus, shader);
    m_Neptune = std::make_shared<Body>(m_Sun, Neptune, shader);

    m_FreeCamera = std::make_shared<FreeCamera>(60, 0.01f, 10000);
    m_FreeCamera->SetPosition(DirectX::SimpleMath::Vector3(0, 100, 500));
    m_FreeCamera->SetOrthographic(10, -1000.0f, 10000.0f);

    m_OrbitCamera = std::make_shared<OrbitCamera>(60, 0.01f, 10000);
    m_OrbitCamera->SetOrthographic(10, -1000.0f, 10000.0f);
}

void SolarSystemLayer::OnUpdate(const float deltaTime)
{
    //Prepare Frame
    RenderCommand::Begin();
    auto& window = Application::Get().GetWindow();
    RenderCommand::SetViewport(0, 0, window.GetWidth(), window.GetHeight());
    m_FreeCamera->SetViewportSize(window.GetWidth(), window.GetHeight());
    m_OrbitCamera->SetViewportSize(window.GetWidth(), window.GetHeight());
    window.DisableCursor();

    float color[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    RenderCommand::SetClearColor(color);
    RenderCommand::Clear();

    //Logic
    m_Sun->Update(deltaTime);
    m_Sun->Draw();

    m_Mercury->Update(deltaTime);
    m_Venus->Update(deltaTime);
    m_Earth->Update(deltaTime);
    m_Moon->Update(deltaTime);
    m_Mars->Update(deltaTime);
    m_Jupiter->Update(deltaTime);
    m_Saturn->Update(deltaTime);
    m_Uranus->Update(deltaTime);
    m_Neptune->Update(deltaTime);

    m_FreeCamera->Update(deltaTime);
    m_OrbitCamera->Update(deltaTime);
    SceneCamera& camera = m_UseFreeCamera ? static_cast<SceneCamera&>(*m_FreeCamera) : static_cast<SceneCamera&>(*m_OrbitCamera);
    const CameraData cameraData{ camera.GetViewProjection() };
    m_CameraConstantBuffer->SetData(&cameraData, sizeof CameraData);

    ////Drawing
    m_Sun->Draw();
    m_Mercury->Draw();
    m_Venus->Draw();
    m_Earth->Draw();
    m_Moon->Draw();
    m_Mars->Draw();
    m_Jupiter->Draw();
    m_Saturn->Draw();
    m_Uranus->Draw();
    m_Neptune->Draw();

    if (Input::IsKeyPressed(KeyCode::D1))
        m_OrbitCamera->SetTarget(m_Mercury->GetTransform());
    if (Input::IsKeyPressed(KeyCode::D2))
        m_OrbitCamera->SetTarget(m_Venus->GetTransform());
    if (Input::IsKeyPressed(KeyCode::D3))
        m_OrbitCamera->SetTarget(m_Earth->GetTransform());
    if (Input::IsKeyPressed(KeyCode::D4))
        m_OrbitCamera->SetTarget(m_Mars->GetTransform());
    if (Input::IsKeyPressed(KeyCode::D5))
        m_OrbitCamera->SetTarget(m_Jupiter->GetTransform());
    if (Input::IsKeyPressed(KeyCode::D6))
        m_OrbitCamera->SetTarget(m_Saturn->GetTransform());
    if (Input::IsKeyPressed(KeyCode::D7))
        m_OrbitCamera->SetTarget(m_Uranus->GetTransform());
    if (Input::IsKeyPressed(KeyCode::D8))
        m_OrbitCamera->SetTarget(m_Neptune->GetTransform());
    if (Input::IsKeyPressed(KeyCode::D9))
        m_OrbitCamera->SetTarget(m_Moon->GetTransform());
    if (Input::IsKeyPressed(KeyCode::D0))
        m_OrbitCamera->SetTarget(m_Sun->GetTransform());

    if (Input::IsKeyPressed(KeyCode::Tab))
        m_UseFreeCamera = !m_UseFreeCamera;

    if (Input::IsKeyPressed(KeyCode::Y))
    {
        m_FreeCamera->SetProjectionType(SceneCamera::ProjectionType::Orthographic);
        m_OrbitCamera->SetProjectionType(SceneCamera::ProjectionType::Orthographic);
    }
	    
    if (Input::IsKeyPressed(KeyCode::U))
    {
        m_FreeCamera->SetProjectionType(SceneCamera::ProjectionType::Perspective);
        m_OrbitCamera->SetProjectionType(SceneCamera::ProjectionType::Perspective);
    }
   
    auto isOrtho = camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic;

    if (isOrtho)
    {
        camera.SetProjectionType(SceneCamera::ProjectionType::Perspective);
        const CameraData skyboxCameraData{ camera.GetViewProjection() };
        m_CameraConstantBuffer->SetData(&skyboxCameraData, sizeof CameraData);
        camera.SetProjectionType(SceneCamera::ProjectionType::Orthographic);
    }

    m_Skybox->Draw(nullptr);


    //End frame
    RenderCommand::End();
}

void SolarSystemLayer::OnEvent(Event& event)
{
}