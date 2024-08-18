#pragma once

namespace LevEngine
{
    struct ModelNode
    {
        UUID MeshUUID = 0;
        Matrix Transform{};
        String Name{};
        Vector<ModelNode*> Children{};
    };
}