#include "levpch.h"
#include "DispatchCommand.h"

#include "DispatchCommandBase.h"

namespace LevEngine
{
    Ref<DispatchCommandBase> DispatchCommand::s_ConcreteCommand = DispatchCommandBase::Create();
    
    void DispatchCommand::Dispatch(const uint32_t groupX, const uint32_t groupY, const uint32_t groupZ)
    {
        s_ConcreteCommand->Dispatch(groupX, groupY, groupZ);
    }
}
