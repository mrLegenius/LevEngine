#include "levpch.h"
#include "MaterialAsset.h"
#include "EngineAssets.h"

namespace LevEngine
{
    Ref<Texture> MaterialAsset::GetIcon() const { return Icons::Material(); }
}
