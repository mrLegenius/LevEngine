#pragma once
#include "RendererComponent.h"
#include "../Renderer/3D/Mesh.h"
#include "../Renderer/D3D11Texture.h"

struct MeshVertex
{
	DirectX::SimpleMath::Vector3 Position;
	DirectX::SimpleMath::Vector3 Normal;
	DirectX::SimpleMath::Vector2 UV;
	//float TexTiling;
};

struct MeshModelBufferData
{
	DirectX::SimpleMath::Matrix Model;
};

class MeshRenderer final : public RendererComponent
{
public:
	explicit MeshRenderer(const std::shared_ptr<D3D11Shader>& shader, 
		const std::shared_ptr<Mesh>& mesh,
		const std::shared_ptr<Texture>& texture,
		float textureTiling = 1)
		: RendererComponent(shader), m_Mesh(mesh), m_Texture(texture)
	{
		m_IndexBuffer = mesh->CreateIndexBuffer();
		m_ModelConstantBuffer = std::make_shared<D3D11ConstantBuffer>(sizeof(MeshModelBufferData), 1);

		const auto verticesCount = m_Mesh->GetVerticesCount();
		const auto meshVertexBuffer = new MeshVertex[verticesCount];

		for (uint32_t i = 0; i < verticesCount; i++)
		{
			meshVertexBuffer[i].Position = m_Mesh->GetVertex(i);
			meshVertexBuffer[i].Normal = m_Mesh->GetNormal(i);
			meshVertexBuffer[i].UV = m_Mesh->GetUV(i) * textureTiling;
			//meshVertexBufferBase[i].TexTiling = 1;
		}

		m_VertexBuffer = mesh->CreateVertexBuffer(shader->GetLayout(), reinterpret_cast<float*>(meshVertexBuffer));

		delete[] meshVertexBuffer;
	}

	~MeshRenderer() override = default;

	void Bind() const override
	{
		m_Texture->Bind();
		RendererComponent::Bind();
	}

protected:
	void Prepare(const std::shared_ptr<Transform>& transform) override
	{
		const MeshModelBufferData data = { transform->GetModel() };
		m_ModelConstantBuffer->SetData(&data, sizeof(MeshModelBufferData));
	}

private:
	std::shared_ptr<Mesh> m_Mesh;
	std::shared_ptr<Texture> m_Texture;
	std::shared_ptr<D3D11ConstantBuffer> m_ModelConstantBuffer;
};
