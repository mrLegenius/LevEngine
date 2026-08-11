#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
    struct LEV_API ModelNode
    {
        UUID MeshUUID = 0;
        UUID MaterialUUID = 0;
        Matrix Transform{};
        String Name{};
        Vector<ModelNode*> Children{};
    };
}