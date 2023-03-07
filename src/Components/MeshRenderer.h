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
		const std::shared_ptr<Texture>& texture,
		const bool isStatic = false)
		: RendererComponent(shader), m_Mesh(mesh), m_Texture(texture), m_Static(isStatic)
	{
		m_VertexBuffer = mesh->CreateVertexBuffer(shader->GetLayout());
		m_IndexBuffer = mesh->CreateIndexBuffer();
		m_MeshVertexBuffer = new MeshVertex[m_Mesh->GetVerticesCount()];
	}

	~MeshRenderer() override
	{
		delete[] m_MeshVertexBuffer;
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

		if (!m_Prepared || !m_Static)
		{
			const auto model = transform->GetModel();
			for (uint32_t i = 0; i < verticesCount; i++)
			{
				m_MeshVertexBuffer[i].Position = DirectX::SimpleMath::Vector3::Transform(m_Mesh->GetVertex(i), model);
				m_MeshVertexBuffer[i].Normal = m_Mesh->GetNormal(i);
				m_MeshVertexBuffer[i].UV = m_Mesh->GetUV(i);
				//meshVertexBufferBase[i].TexIndex = 0;
				//meshVertexBufferBase[i].TexTiling = 1;
			}
			//m_Prepared = true;
		}

		m_VertexBuffer->SetData(m_MeshVertexBuffer, sizeof(MeshVertex) * verticesCount);
	}

private:
	std::shared_ptr<Mesh> m_Mesh;
	MeshVertex* m_MeshVertexBuffer;
	std::shared_ptr<Texture> m_Texture;

	bool m_Static;
	bool m_Prepared = false;
};
