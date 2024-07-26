#include "levpch.h"
#include <d3d11.h>

#include "D3D11RenderCommands.h"

#include "Renderer/Pipeline/IndexBuffer.h"

namespace LevEngine
{
extern ID3D11DeviceContext* context;

void D3D11RenderCommands::DrawIndexed(const Ref<IndexBuffer>& indexBuffer)
{
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	indexBuffer->Bind();
	context->DrawIndexed(indexBuffer->GetCount(), 0, 0);
}

void D3D11RenderCommands::DrawFullScreenQuad()
{
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->Draw(4, 0);
}

void D3D11RenderCommands::DrawLineList(const Ref<IndexBuffer>& indexBuffer)
{
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	indexBuffer->Bind();
	context->DrawIndexed(indexBuffer->GetCount(), 0, 0);
}

void D3D11RenderCommands::DrawLineStrip(const uint32_t vertexCount)
{
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
	context->Draw(vertexCount, 0);
}

void D3D11RenderCommands::DrawPointList(const uint32_t count)
{
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->Draw(count, 0);
}
}
