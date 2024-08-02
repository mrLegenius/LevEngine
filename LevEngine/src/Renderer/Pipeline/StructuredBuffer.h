#pragma once
#include "CPUAccess.h"
#include "UAVType.h"

namespace LevEngine
{
    enum class ShaderType;

    class StructuredBuffer
    {
    public:
        virtual ~StructuredBuffer() = default;

        static Ref<StructuredBuffer> Create(const void* data, size_t count, uint32_t stride, CPUAccess cpuAccess = CPUAccess::None, bool uav = false, UAVType uavType = UAVType::None);

        virtual bool Bind(unsigned id, ShaderType shaderType, bool readWrite, uint32_t counterValue = -1) = 0;
        virtual void Unbind(unsigned id, ShaderType shaderType, bool readWrite) const = 0;

        virtual void Clear() const = 0;
        virtual uint32_t GetCounterValue() const = 0;

        [[nodiscard]] uint32_t GetElementCount() const { return m_ElementsCount; }

    protected:
        StructuredBuffer(
            const size_t count,
            const uint32_t stride,
            const CPUAccess cpuAccess = CPUAccess::None)
            : m_Stride(stride)
            , m_ElementsCount(count)
            , m_CPUAccess(cpuAccess) { }


        // The system data buffer
        typedef Vector<uint8_t> BufferType;
        BufferType m_Data;

        uint32_t m_Stride{};

        size_t m_ElementsCount{};

        // Marked dirty if the contents of the buffer differ
        // from what is stored on the GPU
        bool m_IsDirty{};

        // Does this buffer require GPU write access
        // If so, it must be bound as a UAV instead of an SRV
        bool m_UAV{};

        // Requires CPU read/write access
        bool m_Dynamic{};
        CPUAccess m_CPUAccess{};
    };

}
