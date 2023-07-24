#pragma once
#include <cstdint>
namespace LevEngine
{
enum class ClearFlags : uint8_t
{
    Color = 1 << 0,
    Depth = 1 << 1,
    Stencil = 1 << 2,
    DepthStencil = Depth | Stencil,
    All = Color | Depth | Stencil,
};
}