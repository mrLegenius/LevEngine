#include "levpch.h"
#include <d3d11.h>

#include "D3D11RenderCommands.h"

#include "Renderer/Pipeline/IndexBuffer.h"

namespace LevEngine
{
	D3D11RenderCommands::D3D11RenderCommands(ID3D11Device2* device)
	{
		device->GetImmediateContext2(&m_DeviceContext);
	}

void D3D11RenderCommands::DrawIndexed(const Ref<IndexBuffer>& indexBuffer)
{
	m_DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	indexBuffer->Bind();
	m_DeviceContext->DrawIndexed(indexBuffer->GetCount(), 0, 0);
}

void D3D11RenderCommands::DrawFullScreenQuad()
{
	m_DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	m_DeviceContext->Draw(4, 0);
}

void D3D11RenderCommands::DrawLineList(const Ref<IndexBuffer>& indexBuffer)
{
	m_DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	indexBuffer->Bind();
	m_DeviceContext->DrawIndexed(indexBuffer->GetCount(), 0, 0);
}

void D3D11RenderCommands::DrawLineStrip(const uint32_t vertexCount)
{
	m_DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
	m_DeviceContext->Draw(vertexCount, 0);
}

void D3D11RenderCommands::DrawPointList(const uint32_t count)
{
	m_DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	m_DeviceContext->Draw(count, 0);
}
}
