#pragma once
#include <d3d11_2.h>

#include "Renderer/RenderDebugEvent.h"

namespace LevEngine
{
    class D3D11RenderDebugEvent final : public RenderDebugEvent
    {
    public:
        D3D11RenderDebugEvent(ID3D11Device2* device);
        ~D3D11RenderDebugEvent() override;

        void StartEvent(String label) override;
        void EndEvent() override;
    private:
        ID3DUserDefinedAnnotation* m_UserDefinedAnnotation{};
        ID3D11DeviceContext2* m_DeviceContext;
    };
}
