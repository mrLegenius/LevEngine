#pragma once

namespace LevEngine
{
    enum class ShaderType;

    class ConstantBuffer
    {
    public:
        explicit ConstantBuffer(const uint32_t size) : ConstantBuffer(size, 0)
        {
        }

        explicit ConstantBuffer(const uint32_t size, const uint32_t slot) : m_Size(size), m_Slot(slot)
        {
        }

        virtual ~ConstantBuffer() = default;

        static Ref<ConstantBuffer> Create(uint32_t size, uint32_t slot = 0);

        virtual void SetData(const void* data, uint32_t size) const = 0;
        void SetData(const void* data) const { SetData(data, 0); }

        virtual void Bind(ShaderType shaderType) = 0;
        virtual void Bind(uint32_t slot, ShaderType shaderType) = 0;
        virtual void Unbind(uint32_t slot, ShaderType shaderType) = 0;

    protected:
        uint32_t m_Size;
        uint32_t m_Slot;
    };
}
