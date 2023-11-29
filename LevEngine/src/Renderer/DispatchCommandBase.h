#pragma once

namespace LevEngine
{
    class DispatchCommandBase
    {
    public:
        virtual ~DispatchCommandBase() = default;
        static Ref<DispatchCommandBase> Create();
        virtual void Dispatch(uint32_t groupX, uint32_t groupY, uint32_t groupZ) = 0;
    };
}