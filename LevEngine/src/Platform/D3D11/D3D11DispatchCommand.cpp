#include "levpch.h"
#include "D3D11DispatchCommand.h"

#include <d3d11.h>

#include "D3D11ConstantBuffer.h"
#include "Renderer/Pipeline/ConstantBuffer.h"

namespace LevEngine
{
    D3D11DispatchCommand::D3D11DispatchCommand(ID3D11Device2* device)
    {
        device->GetImmediateContext2(&m_DeviceContext);
    }

    void D3D11DispatchCommand::Dispatch(const uint32_t groupX, const uint32_t groupY, const uint32_t groupZ)
    {
        m_DeviceContext->Dispatch(groupX, groupY, groupZ);
    }

    void D3D11DispatchCommand::DispatchIndirect(const Ref<ConstantBuffer>& constantBuffer) const
    {
        auto buffer = CastRef<D3D11ConstantBuffer>(constantBuffer);
        m_DeviceContext->DispatchIndirect(buffer->GetBuffer(), 0);
    }
}
