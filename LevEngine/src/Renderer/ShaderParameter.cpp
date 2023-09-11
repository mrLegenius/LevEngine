#include "levpch.h"
#include "ShaderParameter.h"

namespace LevEngine
{
void ShaderParameter::Set(const Ref<ConstantBuffer>& constantBuffer)
{
	m_ConstantBuffer = constantBuffer;
}

void ShaderParameter::Set(const Ref<Texture>& texture)
{
	m_Texture = texture;
}

void ShaderParameter::Set(const Ref<SamplerState>& sampler)
{
	m_SamplerState = sampler;
}

void ShaderParameter::Set(const Ref<StructuredBuffer>& structuredBuffer)
{
	m_StructuredBuffer = structuredBuffer;
}

void ShaderParameter::Bind() const
{
    if (const Ref<ConstantBuffer> constantBuffer = m_ConstantBuffer.lock())
    {
        constantBuffer->Bind(m_SlotId, m_ShaderType);
    }
    if (const Ref<Texture> texture = m_Texture.lock())
    {
        texture->Bind(m_SlotId, m_ShaderType);
    }
    if (const Ref<SamplerState> samplerState = m_SamplerState.lock())
    {
        samplerState->Bind(m_SlotId, m_ShaderType);
    }
    if (const Ref<StructuredBuffer> buffer = m_StructuredBuffer.lock())
    {
        //TODO: Bind Structured Buffer
        //buffer->Bind(m_SlotId, m_ShaderType);
    }
}

void ShaderParameter::Unbind() const
{
    if (const Ref<ConstantBuffer> constantBuffer = m_ConstantBuffer.lock())
    {
        constantBuffer->Unbind(m_SlotId, m_ShaderType);
    }
    if (const Ref<Texture> texture = m_Texture.lock())
    {
        texture->Unbind(m_SlotId, m_ShaderType);
    }
    if (const Ref<SamplerState> samplerState = m_SamplerState.lock())
    {
        samplerState->Unbind(m_SlotId, m_ShaderType);
    }
    if (const Ref<StructuredBuffer> buffer = m_StructuredBuffer.lock())
    {
        //TODO: Unbind Structured Buffer
        //buffer->Bind(m_SlotId, m_ShaderType);
    }

}
}