#pragma once
#include "Assets.h"
#include "Mesh.h"

#include "Renderer/IndexBuffer.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexBuffer.h"

namespace LevEngine
{
class SkyboxMesh
{
    struct SkyboxVertex
    {
        Vector3 Position;
    };

public:
    explicit SkyboxMesh(const Ref<Shader>& shader)
	{
        const auto mesh = Mesh::CreateCube();
        IndexBuffer = mesh->CreateIndexBuffer();
        VertexBuffer = mesh->CreateVertexBuffer(shader->GetLayout());

        const auto verticesCount = mesh->GetVerticesCount();
        auto* meshVertexBufferBase = new SkyboxVertex[verticesCount];

        for (uint32_t i = 0; i < verticesCount; i++)
	        meshVertexBufferBase[i].Position = mesh->GetVertex(i);

        VertexBuffer->SetData(meshVertexBufferBase, sizeof(SkyboxVertex) * verticesCount);
        delete[] meshVertexBufferBase;
	}

    Ref<VertexBuffer> VertexBuffer;
    Ref<IndexBuffer> IndexBuffer;
};
}