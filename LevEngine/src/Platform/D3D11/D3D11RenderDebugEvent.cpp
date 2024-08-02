#include "levpch.h"
#include "D3D11RenderDebugEvent.h"

namespace LevEngine
{
    D3D11RenderDebugEvent::D3D11RenderDebugEvent(ID3D11Device2* device)
    {
        device->GetImmediateContext2(&m_DeviceContext);
    }

    D3D11RenderDebugEvent::~D3D11RenderDebugEvent()
    {
        if (m_UserDefinedAnnotation)
            m_UserDefinedAnnotation->Release();
        
        m_UserDefinedAnnotation = nullptr;
    }

    void D3D11RenderDebugEvent::StartEvent(String label)
    {
        if (!m_UserDefinedAnnotation)
            m_DeviceContext->QueryInterface<ID3DUserDefinedAnnotation>(&m_UserDefinedAnnotation);
        
        const auto wideString = std::wstring(label.begin(), label.end());
        m_UserDefinedAnnotation->BeginEvent(wideString.c_str());
    }

    void D3D11RenderDebugEvent::EndEvent()
    {
        m_UserDefinedAnnotation->EndEvent();
    }
}
