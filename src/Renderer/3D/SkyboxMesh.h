#pragma once
#include "Assets.h"
#include "Mesh.h"

#include "Kernel/PointerUtils.h"
#include "Renderer/D3D11IndexBuffer.h"
#include "Renderer/D3D11Shader.h"
#include "Renderer/D3D11VertexBuffer.h"

class SkyboxMesh
{
    struct SkyboxVertex
    {
        Vector3 Position;
    };

public:
	SkyboxMesh(const Ref<D3D11Shader> shader)
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

    Ref<D3D11VertexBuffer> VertexBuffer;
    Ref<D3D11IndexBuffer> IndexBuffer;
};
