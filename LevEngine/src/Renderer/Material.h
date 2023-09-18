#pragma once
#include "Shader.h"
#include "ConstantBuffer.h"

namespace LevEngine
{
class Material
{
public:
    virtual ~Material() = default;

    virtual void Bind(const Ref<Shader>& shader);
    virtual void Unbind(const Ref<Shader>& shader);

protected:
    explicit Material(const size_t gpuDataSize)
        : m_ConstantBuffer(ConstantBuffer::Create(gpuDataSize)) { }

    virtual void* GetGPUData() = 0;
    
    Ref<ConstantBuffer> m_ConstantBuffer;

    bool m_IsDirty{true};
};
}