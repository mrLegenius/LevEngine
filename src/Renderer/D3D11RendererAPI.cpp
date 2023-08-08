#include "pch.h"
#include <d3d11.h>

#include "D3D11RendererAPI.h"

namespace LevEngine
{
extern ID3D11DeviceContext* context;

void D3D11RendererAPI::DrawIndexed(
	const Ref<VertexBuffer>& vertexBuffer,
	const Ref<IndexBuffer>& indexBuffer)
{
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	indexBuffer->Bind();
	vertexBuffer->Bind();
	context->DrawIndexed(indexBuffer->GetCount(), 0, 0);
}

void D3D11RendererAPI::DrawFullScreenQuad()
{
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->Draw(4, 0);
}


void D3D11RendererAPI::DrawPointList(const uint32_t count)
{
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->Draw(count, 0);
}
}
