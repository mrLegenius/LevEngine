#pragma once
#include <SimpleMath.h>

#include "RendererComponent.h"
#include "../Renderer/3D/Mesh.h"

struct SkyboxVertex
{
	Vector3 Position;
};

class SkyboxRenderer final : public RendererComponent
{
public:
	explicit SkyboxRenderer(const Ref<Shader>& shader, const std::string paths[6])
		: SkyboxRenderer(shader, Texture::CreateTextureCube(paths))
	{
	}

	explicit SkyboxRenderer(const Ref<Shader>& shader, const Ref<Texture>& texture)
		: RendererComponent(shader),
		m_Texture(texture),
		m_Mesh(Mesh::CreateCube())
	{
		m_VertexBuffer = m_Mesh->CreateVertexBuffer(shader->GetLayout());
		m_IndexBuffer = m_Mesh->CreateIndexBuffer();
	}

protected:
	void Prepare(const Transform&) override
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
	void Draw(const Transform& transform) override
	{
		RenderCommand::SetDepthFunc(DepthFunc::LessOrEqual);
		RendererComponent::Draw(transform);
		RenderCommand::SetDepthFunc(DepthFunc::Less);
	}

private:
	Ref<Texture> m_Texture;
	Ref<Mesh> m_Mesh;
};
