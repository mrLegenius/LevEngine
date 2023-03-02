#include "SolarSystemLayer.h"

#include <iostream>

#include "CircularMovement.h"
#include "../Components/Transform.h"
#include "../Renderer/3D/Mesh.h"
#include "../Renderer/RenderCommand.h"
#include "../Renderer/D3D11Texture.h"
#include "../Kernel/Application.h"

#include "../Pong/GameObject.h"
void DrawMesh(const DirectX::SimpleMath::Matrix& transform, const Mesh& mesh, const std::shared_ptr<D3D11Shader>& shader, const std::shared_ptr<D3D11Texture2D>& texture);

struct CameraData
{
    DirectX::SimpleMath::Matrix ViewProjection;
};

class Body
{
public:
	explicit Body(const std::shared_ptr<Body>& pivotBody,
        const std::shared_ptr<CircularMovement>& movement,
        const float radius,
        const std::string& textureFilepath)
		: m_PivotBody(pivotBody),
		m_Transform(std::make_shared<Transform>()),
		m_Movement(movement),
		m_Texture(std::make_shared<D3D11Texture2D>(textureFilepath))
	{
        m_Mesh = Mesh::CreateSphere(45);
        m_Transform->scale = DirectX::SimpleMath::Vector3{ radius, radius, radius };
	}

	void Update(const float deltaTime) const
	{
		m_Movement->Update(deltaTime);
	}

    std::shared_ptr<Transform>& GetTransform() { return m_Transform; }
    std::shared_ptr<Mesh>& GetMesh() { return m_Mesh; }
    std::shared_ptr<D3D11Texture2D>& GetTexture() { return m_Texture; }

private:
	std::shared_ptr<Body> m_PivotBody;
    std::shared_ptr<Transform> m_Transform;
    std::shared_ptr<Mesh> m_Mesh;
	std::shared_ptr<CircularMovement> m_Movement;
    std::shared_ptr<D3D11Texture2D> m_Texture;
};

void SolarSystemLayer::OnAttach()
{
    m_MeshShader = std::make_shared<D3D11Shader>("./Shaders/Unlit.hlsl");
    m_MeshShader->SetLayout({
    { ShaderDataType::Float3, "POSITION" },
        { ShaderDataType::Float3, "NORMAL" },
        { ShaderDataType::Float2, "UV" },
        //{ ShaderDataType::Float, "a_TexIndex" },
        //{ ShaderDataType::Float, "a_TexTiling" },
    });

    m_CameraConstantBuffer = std::make_shared<D3D11ConstantBuffer>(sizeof CameraData);

    m_TestBody = std::make_shared<Body>(nullptr, std::make_shared<CircularMovement>(), 1, "./resources/Textures/earth.jpg");
    m_TestBody->GetTransform()->position.z = -10;
    m_FreeCamera = std::make_shared<FreeCamera>(45, 0.01f, 10000);
}

void SolarSystemLayer::OnUpdate()
{
    RenderCommand::Begin();
    auto& window = Application::Get().GetWindow();
    RenderCommand::SetViewport(0, 0, window.GetWidth(), window.GetHeight());
    m_FreeCamera->SetViewportSize(window.GetWidth(), window.GetHeight());
    window.DisableCursor();

    float color[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    RenderCommand::SetClearColor(color);
    RenderCommand::Clear();

    constexpr auto deltaTime = 1.f / 144.f;
    m_FreeCamera->Update(deltaTime);
    static float move = 0;
    //m_TestBody->GetTransform()->position.z = sin(move);
    m_TestBody->GetTransform()->rotation.y += deltaTime;

	const CameraData cameraData{ m_FreeCamera->GetViewProjection()};
    m_CameraConstantBuffer->SetData(&cameraData, sizeof CameraData);

    move += deltaTime;

    DrawMesh(m_TestBody->GetTransform()->GetModel(), *m_TestBody->GetMesh(), m_MeshShader, m_TestBody->GetTexture());

    RenderCommand::End();
}

void SolarSystemLayer::OnEvent(Event& event)
{
}

struct MeshVertex
{
    DirectX::SimpleMath::Vector3 Position;
    DirectX::SimpleMath::Vector3 Normal;
    DirectX::SimpleMath::Vector2 UV;
    //float TexIndex;
    //float TexTiling;
};

void DrawMesh(const DirectX::SimpleMath::Matrix& transform, const Mesh& mesh, const std::shared_ptr<D3D11Shader>& shader, const std::shared_ptr<D3D11Texture2D>& texture)
{
    const std::shared_ptr<D3D11VertexBuffer> vertexBuffer = mesh.CreateVertexBuffer(shader->GetLayout());
    const std::shared_ptr<D3D11IndexBuffer> indexBuffer = mesh.CreateIndexBuffer();

    const auto verticesCount = mesh.GetVerticesCount();
    auto* meshVertexBufferBase = new MeshVertex[verticesCount];
    for (uint32_t i = 0; i < verticesCount; i++)
    {
        meshVertexBufferBase[i].Position = DirectX::SimpleMath::Vector3::Transform(mesh.GetVertex(i), transform);
        meshVertexBufferBase[i].Normal = mesh.GetNormal(i);
        meshVertexBufferBase[i].UV = mesh.GetUV(i);
        //meshVertexBufferBase[i].TexIndex = 0;
        //meshVertexBufferBase[i].TexTiling = 1;
    }

    vertexBuffer->SetData(meshVertexBufferBase);
    texture->Bind();
    
    shader->Bind();

    RenderCommand::DrawIndexed(vertexBuffer, indexBuffer);
}
