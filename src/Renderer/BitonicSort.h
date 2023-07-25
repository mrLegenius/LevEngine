#pragma once
#include <cstdint>

#include "Assets.h"
#include "ConstantBuffer.h"
#include "D3D11Shader.h"
#include "D3D11StructuredBuffer.h"

namespace LevEngine
{
class BitonicSort
{
    struct ConstantsGPUData
    {
        int Level;
        int LevelMask;
        int Width;
        int Height;
    };

    static constexpr uint32_t BitonicBlockSize = 512;
    static constexpr uint32_t TransposeBlockSize = 16;

    Ref<D3D11Shader> m_BitonicSortCS;
    Ref<D3D11Shader> m_BitonicTransposeCS;
    Ref<ConstantBuffer> m_ConstantsBuffer;

    int m_NumElements;
public:
    explicit BitonicSort(int numElements);

    void Sort(Ref<D3D11StructuredBuffer> inBuffer, Ref<D3D11StructuredBuffer> tempBuffer) const;
private:

    void SetGPUSortConstants(uint32_t level, uint32_t levelMask, uint32_t width, uint32_t height) const;
};
}