#pragma once

namespace LevEngine
{
    class RenderDebugEvent
    {
    public:
        static void Start(const String& label);
        static void End();
        
    protected:
        virtual ~RenderDebugEvent() = default;
        virtual void StartEvent(String label) = 0;
        virtual void EndEvent() = 0;

    private:
        static Ref<RenderDebugEvent> Create();
        static inline Ref<RenderDebugEvent> s_RendererAPI = Create();
    };
}
