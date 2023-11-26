#pragma once
#include "Asset.h"

namespace LevEngine
{
    class ScriptAsset :public Asset
    {
    public:
        ScriptAsset(const Path& path, const UUID uuid) : Asset(path, uuid) {}
        
    };
}

