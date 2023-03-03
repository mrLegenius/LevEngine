#pragma once
#include "RendererComponent.h"
#include "../Renderer/3D/Mesh.h"
#include "../Renderer/D3D11Texture.h"

struct MeshVertex
{
	DirectX::SimpleMath::Vector3 Position;
	DirectX::SimpleMath::Vector3 Normal;
	DirectX::SimpleMath::Vector2 UV;
	//float TexIndex;
	//float TexTiling;
};

class MeshRenderer final : public RendererComponent
{
public:
	explicit MeshRenderer(const std::shared_ptr<D3D11Shader>& shader, 
		const std::shared_ptr<Mesh>& mesh,
		const std::shared_ptr<Texture>& texture)
		: RendererComponent(shader), m_Mesh(mesh), m_Texture(texture)
	{
		m_VertexBuffer = mesh->CreateVertexBuffer(shader->GetLayout());
		m_IndexBuffer = mesh->CreateIndexBuffer();
	}

	void Bind() const override
	{
		m_Texture->Bind();
		RendererComponent::Bind();
	}

protected:
	void Prepare(const std::shared_ptr<Transform>& transform) override
	{
		const auto verticesCount = m_Mesh->GetVerticesCount();
		auto* meshVertexBufferBase = new MeshVertex[verticesCount];

		for (uint32_t i = 0; i < verticesCount; i++)
		{
			meshVertexBufferBase[i].Position = DirectX::SimpleMath::Vector3::Transform(m_Mesh->GetVertex(i), transform->GetModel());
			meshVertexBufferBase[i].Normal = m_Mesh->GetNormal(i);
			meshVertexBufferBase[i].UV = m_Mesh->GetUV(i);
			//meshVertexBufferBase[i].TexIndex = 0;
			//meshVertexBufferBase[i].TexTiling = 1;
		}

		m_VertexBuffer->SetData(meshVertexBufferBase, sizeof(MeshVertex) * verticesCount);
		delete[] meshVertexBufferBase;
	}
	
private:
	std::shared_ptr<Mesh> m_Mesh;
	std::shared_ptr<Texture> m_Texture;
};
