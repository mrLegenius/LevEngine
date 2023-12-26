#include "levpch.h"
#include "D3D11ContextSelector.h"

namespace LevEngine
{
    extern ID3D11DeviceContext* context;
    extern ID3D11DeviceContext* deferredContext;
    
    ID3D11DeviceContext* D3D11ContextSelector::GetContext()
    {
        const auto job = vgjs::current_job();
        if (!job) return context;

        if (job->m_thread_index == -1) return context;

        return deferredContext;
    }
}
