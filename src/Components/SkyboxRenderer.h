#pragma once
#include <SimpleMath.h>

#include "RendererComponent.h"
#include "../Renderer/3D/Mesh.h"
#include "../Renderer/D3D11TextureCube.h"

struct SkyboxVertex
{
	DirectX::SimpleMath::Vector3 Position;
};

class SkyboxRenderer final : public RendererComponent
{
public:
	explicit SkyboxRenderer(const std::shared_ptr<D3D11Shader>& shader, const std::string paths[6])
		: RendererComponent(shader),
		m_Texture(std::make_shared<D3D11TextureCube>(paths)),
		m_Mesh(Mesh::CreateCube())
	{
		m_VertexBuffer = m_Mesh->CreateVertexBuffer(shader->GetLayout());
		m_IndexBuffer = m_Mesh->CreateIndexBuffer();
	}

protected:
	void Prepare(const std::shared_ptr<Transform>&) override
	{
		const auto verticesCount = m_Mesh->GetVerticesCount();
		auto* meshVertexBufferBase = new SkyboxVertex[verticesCount];

		for (uint32_t i = 0; i < verticesCount; i++)
		{
			meshVertexBufferBase[i].Position = m_Mesh->GetVertex(i);
		}

		m_VertexBuffer->SetData(meshVertexBufferBase, sizeof(SkyboxVertex) * verticesCount);
		delete[] meshVertexBufferBase;
	}

	void Bind() const override
	{
		m_Texture->Bind();
		RendererComponent::Bind();
	}

public:
	void Draw(const std::shared_ptr<Transform>& transform) override
	{
		RenderCommand::SetDepthFunc(DepthFunc::LessOrEqual);
		RendererComponent::Draw(transform);
		RenderCommand::SetDepthFunc(DepthFunc::Less);
	}

private:
	std::shared_ptr<D3D11TextureCube> m_Texture;
	std::shared_ptr<Mesh> m_Mesh;
};
