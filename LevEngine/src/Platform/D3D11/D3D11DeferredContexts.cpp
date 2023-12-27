#include "levpch.h"
#include "D3D11DeferredContexts.h"

#include <wrl/client.h>

namespace LevEngine
{
    extern ID3D11DeviceContext* context;
    extern Microsoft::WRL::ComPtr<ID3D11Device> device;

    std::mutex mutex;
    void D3D11DeferredContexts::Init(int jobThreadsCount)
    {
        for (int i = 1; i < jobThreadsCount; i++)
        {
            vgjs::schedule(vgjs::Function{[=]
            {
                ID3D11DeviceContext* deferredContext;
                std::lock_guard lock(mutex);
                const auto result = device->CreateDeferredContext(0, &deferredContext);
                LEV_CORE_ASSERT(SUCCEEDED(result), "Failed to create deferred context")
                m_DeferredContexts.push_back(deferredContext);
                m_CommandLists.push_back(nullptr);
            }, vgjs::thread_index_t{ i }});
        }
    }

    void D3D11DeferredContexts::UpdateCommandLists()
    {
        std::lock_guard lock(mutex);
        for (int i = 0; i < m_DeferredContexts.size(); i++)
        {
            vgjs::schedule(vgjs::Function{[=]
            {
                const auto& context = m_DeferredContexts[i];
                auto& list = m_CommandLists[i];
                
                const auto res = context->FinishCommandList(false, &list);
                LEV_CORE_ASSERT(SUCCEEDED(res), "Failed to finish command list");
                
            }, vgjs::thread_index_t{ i + 1 }});
        }
    }

    void D3D11DeferredContexts::ExecuteCommands()
    {
        std::lock_guard lock(mutex);

        for (int i = 0; i < m_CommandLists.size(); i++)
        {
            if (const auto list = m_CommandLists[i])
            {
                context->ExecuteCommandList(list, true);
            }
        }

        for (int i = 0; i < m_CommandLists.size(); i++)
        {
            vgjs::schedule(vgjs::Function{[=]
            {
                if (const auto& list = m_CommandLists[i])
                {
                    list->Release();
                }
            }, vgjs::thread_index_t{ i + 1 }});
        }
    }

    ID3D11DeviceContext* D3D11DeferredContexts::GetContext()
    {
        const auto job = vgjs::current_job();
        if (!job) return context;

        if (job->m_thread_index == 0) return context;
        
        return m_DeferredContexts[job->m_thread_index.value - 1];
    }
}
