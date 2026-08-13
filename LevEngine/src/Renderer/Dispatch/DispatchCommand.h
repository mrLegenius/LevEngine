#pragma once

#include "Kernel/Core.h"
#include "DispatchCommands.h"

namespace LevEngine
{
    class Application;
}

namespace LevEngine
{
    class DispatchCommands;

    class LEV_API DispatchCommand
    {
    public:
        static void Dispatch(uint32_t groupX, uint32_t groupY, uint32_t groupZ);
        

    private:
        friend Application;
        static void Init() { s_ConcreteCommand = DispatchCommands::Create(); }
        //Has to be called while the render device is still alive, see TextureLibrary::Shutdown
        static void Shutdown() { s_ConcreteCommand.reset(); }
        inline static Ref<DispatchCommands> s_ConcreteCommand{};
    };
}

