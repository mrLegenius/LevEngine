#pragma once

namespace LevEngine
{
    class StructuredBuffer;
    class ConstantBuffer;
    class Shader;

    class BitonicSort
    {
        struct ConstantsGPUData
        {
            int Level;
            int LevelMask;
            int Width;
            int Height;
        };
        
        Ref<Shader> m_BitonicSortCS;
        Ref<Shader> m_BitonicTransposeCS;
        Ref<ConstantBuffer> m_ConstantsBuffer;

        int m_NumElements;

    public:
        explicit BitonicSort(int numElements);

        void Sort(const Ref<StructuredBuffer>& inBuffer, const Ref<StructuredBuffer>& tempBuffer) const;

    private:
        void SetGPUSortConstants(uint32_t level, uint32_t levelMask, uint32_t width, uint32_t height) const;
    };
}
