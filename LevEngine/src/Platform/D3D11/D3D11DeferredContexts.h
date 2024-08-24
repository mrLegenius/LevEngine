#pragma once
#include "D3D11RenderContext.h"

namespace LevEngine
{
    class D3D11DeferredContexts
    {
    public:
        static void SetRenderDevice(const Ref<RenderDevice>& renderDevice);
        static void Init(const int jobThreadsCount);
        static ID3D11DeviceContext* GetContext();
        static void UpdateCommandLists();
        static void ExecuteCommands();
    private:
        inline static Vector<ID3D11DeviceContext*> m_DeferredContexts;
        inline static Vector<ID3D11CommandList*> m_CommandLists;

        inline static ID3D11Device2* m_Device{};
        inline static ID3D11DeviceContext2* m_DeviceContext{};
    };
}

