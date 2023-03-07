#include "KatamariLayer.h"

#include "ObjLoader.h"
#include "../Kernel/Application.h"
#include "../Pong/GameObject.h"
#include "../Components/MeshRenderer.h"
#include "../SolarSystem/OrbitCamera.h"
#include "../Components/SkyboxRenderer.h"

struct CameraData
{
    DirectX::SimpleMath::Matrix ViewProjection;
};

std::vector<std::shared_ptr<GameObject>> objects;

std::shared_ptr<SkyboxRenderer> m_Skybox;

std::shared_ptr<GameObject> CreateGameObjectFromObj(
    const std::string& meshFile, 
    const std::string& textureFile,
    std::shared_ptr<D3D11Shader> shader)
{
    auto mesh = ObjLoader().LoadMesh(meshFile);
    auto texture = std::make_shared<D3D11Texture2D>(textureFile);
    auto renderer = std::make_shared<MeshRenderer>(shader, mesh, texture, true);
    return std::make_shared<GameObject>(renderer);
}

void KatamariLayer::OnAttach()
{
    auto skyboxShader = std::make_shared<D3D11Shader>("./Shaders/Skybox.hlsl");
    skyboxShader->SetLayout({
    {  ShaderDataType::Float3, "POSITION" },
        });

    auto shader = std::make_shared<D3D11Shader>("./Shaders/Unlit.hlsl");
    shader->SetLayout({
    { ShaderDataType::Float3, "POSITION" },
        { ShaderDataType::Float3, "NORMAL" },
        { ShaderDataType::Float2, "UV" },
        //{ ShaderDataType::Float, "a_TexIndex" },
        //{ ShaderDataType::Float, "a_TexTiling" },
        });

    /*auto gear = CreateGameObjectFromObj(
	"./resources/Models/gear.obj",
	"./resources/Textures/gear.png", shader);
    objects.emplace_back(gear);*/

    auto log = CreateGameObjectFromObj(
        "./resources/Models/log.obj",
        "./resources/Textures/log.jpg", shader);
    objects.emplace_back(log);

    auto rock = CreateGameObjectFromObj(
        "./resources/Models/rock.obj",
        "./resources/Textures/rock.tga", shader);
    objects.emplace_back(rock);

    auto mesh = Mesh::CreateCube();
    auto texture = std::make_shared<D3D11Texture2D>("./resources/Textures/gear.png");
    auto renderer = std::make_shared<MeshRenderer>(shader, mesh, texture);
	auto floor = std::make_shared<GameObject>(renderer);
    floor->GetTransform()->SetScale(DirectX::SimpleMath::Vector3(100, 0.1f, 100));
    objects.emplace_back(floor);

    std::string textures[6] = {
        "./resources/Textures/LightBlueSkybox/left.png", //left
        "./resources/Textures/LightBlueSkybox/right.png", //right
        "./resources/Textures/LightBlueSkybox/top.png", //top
        "./resources/Textures/LightBlueSkybox/bottom.png", //bottom
        "./resources/Textures/LightBlueSkybox/back.png", //back
        "./resources/Textures/LightBlueSkybox/front.png", //front
    };
    m_Skybox = std::make_shared<SkyboxRenderer>(skyboxShader, textures);

    m_Camera = std::make_shared<FreeCamera>(45, 0.01f, 10000);
    m_Camera->SetPosition(DirectX::SimpleMath::Vector3(0, 100, 500));
    m_Camera->SetProjectionType(SceneCamera::ProjectionType::Perspective);

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

    m_Skybox->Draw(nullptr);

    //End frame
    RenderCommand::End();
}

void KatamariLayer::OnEvent(Event& event)
{
	
}
