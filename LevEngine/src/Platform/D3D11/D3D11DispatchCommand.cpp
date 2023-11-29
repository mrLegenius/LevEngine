#include "levpch.h"
#include "D3D11DispatchCommand.h"

#include <d3d11.h>

namespace LevEngine
{
extern ID3D11DeviceContext* context;

void D3D11DispatchCommand::Dispatch(const uint32_t groupX, const uint32_t groupY, const uint32_t groupZ)
{
    context->Dispatch(groupX, groupY, groupZ);
}
}