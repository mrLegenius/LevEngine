#include "SolarSystemLayer.h"

#include "CircularMovement.h"
#include "../Renderer/3D/Mesh.h"
#include "../Renderer/RenderCommand.h"
#include "../Renderer/D3D11Texture.h"
#include "../Kernel/Application.h"
#include "../Input/Input.h"

#include "../Pong/GameObject.h"
#include "Body.h"

void DrawMesh(const DirectX::SimpleMath::Matrix& transform, const Mesh& mesh, const std::shared_ptr<D3D11Shader>& shader, const std::shared_ptr<D3D11Texture2D>& texture);

struct CameraData
{
    DirectX::SimpleMath::Matrix ViewProjection;
};

float orbitRadiusScale = 3;

BodyParameters Sun{ 69.634f, 0, 0, 0, 0.2f,"./resources/Textures/sun.jpg" };
BodyParameters Mercury{ 2.44f, 35 * orbitRadiusScale, 0.479f, 0,0.0011f,"./resources/Textures/mercury.jpg" };
BodyParameters Venus{ 6.05f, 67 * orbitRadiusScale, 0.35f, 0,0.0007f,"./resources/Textures/venus.jpg" };
BodyParameters Earth{ 6.37f, 93 * orbitRadiusScale, 0.298f, 0, .1574f,"./resources/Textures/earth.jpg" };
BodyParameters Moon{ 1.7f, 38.4f, 1, 0,.1574f,"./resources/Textures/moon.jpg" };
BodyParameters Mars{ 3.389f, 142 * orbitRadiusScale, 0.241f, 0,0.086f,"./resources/Textures/mars.jpg" };
BodyParameters Jupiter{ 69.91f, 484 * orbitRadiusScale, .131f, 0,4.5583f,"./resources/Textures/jupiter.jpg" };
BodyParameters Saturn{ 58.23f, 889 * orbitRadiusScale, 0.097f, 0,3.6840f,"./resources/Textures/saturn.jpg" };
BodyParameters Uranus{ 25.36f, 1790 * orbitRadiusScale, 0.068f, 0,1.4794f,"./resources/Textures/uranus.jpg" };
BodyParameters Neptune{ 24.62f, 2880 * orbitRadiusScale, 0.54f, 0,.9719f,"./resources/Textures/neptune.jpg" };

void SolarSystemLayer::OnAttach()
{
    auto shader = std::make_shared<D3D11Shader>("./Shaders/Unlit.hlsl");
    shader->SetLayout({
    { ShaderDataType::Float3, "POSITION" },
        { ShaderDataType::Float3, "NORMAL" },
        { ShaderDataType::Float2, "UV" },
        //{ ShaderDataType::Float, "a_TexIndex" },
        //{ ShaderDataType::Float, "a_TexTiling" },
    });

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

    m_OrbitCamera = std::make_shared<OrbitCamera>(60, 0.01f, 10000);
    m_FreeCamera->SetPosition(DirectX::SimpleMath::Vector3(0, 100, 500));
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

    if (m_UseFreeCamera)
    {
        const CameraData cameraData{ m_FreeCamera->GetViewProjection() };
        m_CameraConstantBuffer->SetData(&cameraData, sizeof CameraData);
    }
    else
    {
        const CameraData cameraData{ m_OrbitCamera->GetViewProjection() };
        m_CameraConstantBuffer->SetData(&cameraData, sizeof CameraData);
    }

    //Logic
    m_FreeCamera->Update(deltaTime);
    m_OrbitCamera->Update(deltaTime);
    m_Sun->Update(deltaTime);
    m_Mercury->Update(deltaTime);
    m_Venus->Update(deltaTime);
    m_Earth->Update(deltaTime);
    m_Moon->Update(deltaTime);
    m_Mars->Update(deltaTime);
    m_Jupiter->Update(deltaTime);
    m_Saturn->Update(deltaTime);
    m_Uranus->Update(deltaTime);
    m_Neptune->Update(deltaTime);

    //Drawing
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

    if (Input::IsKeyPressed(KeyCode::Tab))
        m_UseFreeCamera = !m_UseFreeCamera;

    //End frame
    RenderCommand::End();
}

void SolarSystemLayer::OnEvent(Event& event)
{
}