#pragma once
#include "DispatchCommands.h"

namespace LevEngine
{
    class Application;
}

namespace LevEngine
{
    class DispatchCommands;

    class DispatchCommand
    {
    public:
        static void Dispatch(uint32_t groupX, uint32_t groupY, uint32_t groupZ);
        

    private:
        friend Application;
        static void Init() { s_ConcreteCommand = DispatchCommands::Create(); }
        inline static Ref<DispatchCommands> s_ConcreteCommand{};
    };
}

