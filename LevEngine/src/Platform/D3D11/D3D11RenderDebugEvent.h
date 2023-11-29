#pragma once
#include <d3d11_1.h>

#include "Renderer/RenderDebugEvent.h"

namespace LevEngine
{
    class D3D11RenderDebugEvent final : public RenderDebugEvent
    {
    public:
        D3D11RenderDebugEvent() = default;
        ~D3D11RenderDebugEvent() override;

        void StartEvent(String label) override;
        void EndEvent() override;
    private:
        ID3DUserDefinedAnnotation* m_UserDefinedAnnotation{};
    };
}
