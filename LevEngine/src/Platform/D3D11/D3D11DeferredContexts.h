#pragma once
#include "D3D11RendererContext.h"

namespace LevEngine
{
    class D3D11DeferredContexts
    {
    public:
        static void Init(int jobThreadsCount);
        static ID3D11DeviceContext* GetContext();
        static void UpdateCommandLists();
        static void ExecuteCommands();
    private:
        inline static Vector<ID3D11DeviceContext*> m_DeferredContexts;
        inline static Vector<ID3D11CommandList*> m_CommandLists;
    };
}

