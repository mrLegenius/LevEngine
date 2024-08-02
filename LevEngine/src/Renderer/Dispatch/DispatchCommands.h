#pragma once

namespace LevEngine
{
    class DispatchCommands
    {
    public:
        virtual ~DispatchCommands() = default;
        static Ref<DispatchCommands> Create();
        virtual void Dispatch(uint32_t groupX, uint32_t groupY, uint32_t groupZ) = 0;
    };
}