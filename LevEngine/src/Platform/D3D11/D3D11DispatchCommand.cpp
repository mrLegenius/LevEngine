#include "levpch.h"
#include "D3D11DispatchCommand.h"

#include <d3d11.h>

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
}