#pragma once

#include "Kernel/Core.h"
#include "D3D11RenderContext.h"

namespace LevEngine
{
    class LEV_API D3D11DeferredContexts
    {
    public:
        static void SetRenderDevice(const Ref<RenderDevice>& renderDevice);
        static void Init(const int jobThreadsCount);
        //Releases the deferred contexts and the command lists. Has to be called while the render
        //device is still alive, see TextureLibrary::Shutdown
        static void Shutdown();
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

