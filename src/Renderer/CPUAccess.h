#pragma once
namespace LevEngine
{
// CPU Access. Used for textures and Buffers
enum class CPUAccess
{
    None = 0,          // No CPU access to this texture is necessary.
    Read = 1, // CPU reads permitted.
    Write = 2, // CPU writes permitted.
    ReadWrite = Read | Write
};
}