#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
    class Application;
    
    class LEV_API RenderDebugEvent
    {
    public:
        static void Start(const String& label);
        static void End();
        
    protected:
        virtual ~RenderDebugEvent() = default;
        virtual void StartEvent(String label) = 0;
        virtual void EndEvent() = 0;

    private:
        friend Application;
        static void Init() { s_Instance = Create(); }
        //Has to be called while the render device is still alive, see TextureLibrary::Shutdown
        static void Shutdown() { s_Instance.reset(); }
        static Ref<RenderDebugEvent> Create();
        static inline Ref<RenderDebugEvent> s_Instance{};
    };
}
