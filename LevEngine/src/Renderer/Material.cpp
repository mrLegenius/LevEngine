#include "levpch.h"
#include "Material.h"

#include "Shader.h"

namespace LevEngine
{
    void Material::Bind(const Ref<Shader>& shader)
    {
        if (m_IsDirty)
        {
            m_ConstantBuffer->SetData(GetGPUData());
            m_IsDirty = false;
        }
        
        auto parameter = shader->GetShaderParameterByName("MaterialConstantBuffer");
        if (parameter.IsValid())
        {
            parameter.Set(m_ConstantBuffer);
            parameter.Bind();
        }
    }

    void Material::Unbind(const Ref<Shader>& shader)
    {
        const auto parameter = shader->GetShaderParameterByName("MaterialConstantBuffer");
        if (parameter.IsValid())
            parameter.Unbind();
    }
}
