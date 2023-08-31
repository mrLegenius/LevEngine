#include "levpch.h"
#include "Material.h"

namespace LevEngine
{
Material::Material()
{
	m_ConstantBuffer = ConstantBuffer::Create(sizeof GPUData);
}

void Material::Bind(const Ref<Shader>& shader)
{
	if (m_IsDirty)
	{
		m_ConstantBuffer->SetData(&m_Data);
		m_IsDirty = false;
	}

	for (auto [textureType, texture] : m_Textures)
	{
		if (texture)
			texture->Bind(static_cast<uint32_t>(textureType), shader->GetType());
	}

	shader->GetShaderParameterByName("MaterialConstantBuffer").Set(m_ConstantBuffer);
}

void Material::Unbind(const Ref<Shader>& shader)
{
	for (auto [textureType, texture] : m_Textures)
	{
		if (texture)
			texture->Unbind(static_cast<uint32_t>(textureType), shader->GetType());
	}
}
}
