#include "levpch.h"
#include "D3D11DeferredContexts.h"

#include <wrl/client.h>

#include "D3D11RenderDevice.h"

namespace LevEngine
{
    static std::mutex mutex;
    static bool isInitializing = true; 

    enum class State
    {
        NeedUpdate,
        Updating,
        NeedExecute,
        Executing,
    };

    State State;

    void D3D11DeferredContexts::Init(const int jobThreadsCount)
    {
        isInitializing = true;
        vgjs::schedule([=]
        {
            for (int i = 1; i < jobThreadsCount; i++)
            {
                vgjs::schedule(vgjs::Function{
                    [=]
                    {
                        ID3D11DeviceContext* deferredContext;
                        std::lock_guard lock(mutex);
                        const auto result = m_Device->CreateDeferredContext(0, &deferredContext);
                        LEV_CORE_ASSERT(SUCCEEDED(result), "Failed to create deferred context")
                        m_DeferredContexts.push_back(deferredContext);
                        m_CommandLists.push_back(nullptr);
                    },
                    vgjs::thread_index_t{i}
                });
            }

            vgjs::continuation([=]
            {
                isInitializing = false;
            });
        });

        while (isInitializing)
            std::this_thread::sleep_for(microseconds(10));
    }

    void D3D11DeferredContexts::UpdateCommandLists()
    {
        if (State != State::NeedUpdate) return;

        State = State::Updating;
        vgjs::schedule([=]
        {
            for (int i = 0; i < m_DeferredContexts.size(); i++)
            {
                vgjs::schedule(vgjs::Function{
                    [=]
                    {
                        const auto& context = m_DeferredContexts[i];
                        auto& list = m_CommandLists[i];

                        const auto res = context->FinishCommandList(false, &list);
                        LEV_CORE_ASSERT(SUCCEEDED(res), "Failed to finish command list");
                    },
                    vgjs::thread_index_t{i + 1}
                });
            }
            vgjs::continuation([=]
            {
                State = State::NeedExecute;
            });
        });
    }

    void D3D11DeferredContexts::ExecuteCommands()
    {
        if (State != State::NeedExecute) return;

        State = State::Executing;
        for (int i = 0; i < m_CommandLists.size(); i++)
        {
            if (const auto list = m_CommandLists[i])
            {
                m_DeviceContext->ExecuteCommandList(list, true);
            }
        }

        vgjs::schedule([=]
        {
            for (int i = 0; i < m_CommandLists.size(); i++)
            {
                vgjs::schedule(vgjs::Function{
                    [=]
                    {
                        if (auto& list = m_CommandLists[i])
                        {
                            list->Release();
                            list = nullptr;
                        }
                    },
                    vgjs::thread_index_t{i + 1}
                });
            }

            vgjs::continuation([=] { State = State::NeedUpdate; });
        });
    }

    void D3D11DeferredContexts::SetRenderDevice(const Ref<RenderDevice>& renderDevice)
    {
        m_Device = CastRef<D3D11RenderDevice>(renderDevice)->GetDevice().Get();
        m_Device->GetImmediateContext2(&m_DeviceContext);
    }

    ID3D11DeviceContext* D3D11DeferredContexts::GetContext()
    {
        const auto job = vgjs::current_job();
        if (!job) return m_DeviceContext;

        if (job->m_thread_index == 0) return m_DeviceContext;

        return m_DeferredContexts[job->m_thread_index.value - 1];
    }
}
