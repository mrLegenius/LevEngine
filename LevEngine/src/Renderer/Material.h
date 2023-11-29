#pragma once

namespace LevEngine
{
    class ConstantBuffer;
    class Shader;

    class Material
    {
    public:
        virtual ~Material() = default;

        virtual void Bind(const Ref<Shader>& shader);
        virtual void Unbind(const Ref<Shader>& shader);

        [[nodiscard]] virtual bool IsTransparent() = 0;

    protected:
        explicit Material(uint32_t gpuDataSize);

        virtual void* GetGPUData() = 0;

        Ref<ConstantBuffer> m_ConstantBuffer;

        bool m_IsDirty{true};
    };
}
