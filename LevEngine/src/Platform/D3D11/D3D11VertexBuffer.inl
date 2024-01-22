#pragma once
#include "D3D11VertexBuffer.h"
namespace LevEngine
{
    template<typename T>
    D3D11VertexBuffer::D3D11VertexBuffer(const T* data, const uint32_t count, const uint32_t stride)
        : D3D11VertexBuffer(reinterpret_cast<const void*>(data), count, stride) {}
}
