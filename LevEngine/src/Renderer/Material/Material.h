#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
    class ConstantBuffer;
    class Shader;

    // Which permutation of a material's own shader a pass needs. Each flag is a macro the
    // shader gets compiled with -- see the template in LevResources/Shaders/Templates.
    struct LEV_API MaterialShaderVariant
    {
        bool Deferred{};  //<--- LEV_DEFERRED: fill the G-buffer instead of a single target ---<<
        bool Instanced{}; //<--- WITH_INSTANCING ---<<
        bool Animated{};  //<--- WITH_ANIMATIONS ---<<

        [[nodiscard]] uint32_t GetIndex() const
        {
            return (Deferred ? 1u : 0u) | (Instanced ? 2u : 0u) | (Animated ? 4u : 0u);
        }

        static constexpr uint32_t Count = 8;
    };

    class LEV_API Material
    {
    public:
        virtual ~Material() = default;

        virtual void Bind(const Ref<Shader>& shader);
        virtual void Unbind(const Ref<Shader>& shader);

        [[nodiscard]] virtual bool IsTransparent() = 0;

        // The shader this material draws itself with. Null -- the answer for every material the
        // engine ships -- means the pass draws it with the pass' own shader.
        [[nodiscard]] virtual Ref<Shader> GetShader(const MaterialShaderVariant& variant) const { return nullptr; }

    protected:
        explicit Material(uint32_t gpuDataSize);

        //<--- For materials that only learn their buffer size at runtime ---<<
        Material() = default;

        //<--- (Re)creates the constant buffer. A size of 0 leaves the material without one ---<<
        void ResizeGPUData(uint32_t gpuDataSize);

        virtual void* GetGPUData() = 0;

        Ref<ConstantBuffer> m_ConstantBuffer;

        bool m_IsDirty{true};
    };
}
