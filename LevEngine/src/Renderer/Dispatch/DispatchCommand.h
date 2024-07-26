#pragma once

namespace LevEngine
{
    class DispatchCommandBase;

    class DispatchCommand
    {
    public:
        static void Dispatch(uint32_t groupX, uint32_t groupY, uint32_t groupZ);
    private:
        static Ref<DispatchCommandBase> s_ConcreteCommand;
    };
}

