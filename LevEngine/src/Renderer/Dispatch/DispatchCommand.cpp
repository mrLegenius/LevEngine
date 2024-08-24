#include "levpch.h"
#include "DispatchCommand.h"

#include "DispatchCommands.h"

namespace LevEngine
{
    void DispatchCommand::Dispatch(const uint32_t groupX, const uint32_t groupY, const uint32_t groupZ)
    {
        s_ConcreteCommand->Dispatch(groupX, groupY, groupZ);
    }
}
