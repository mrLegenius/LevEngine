#include "levpch.h"
#include "Material.h"

#include "RenderSettings.h"

namespace LevEngine
{
Material::Material()
{
	m_ConstantBuffer = ConstantBuffer::Create(sizeof GPUData, RenderSettings::MaterialSlot);
}

void Material::Bind(const Ref<Shader>& shader)
{
	if (m_IsDirty)
	{
		m_ConstantBuffer->SetData(&m_Data);
		m_IsDirty = false;
	}

	for (auto [textureType, texture] : m_Textures)
		texture->Bind(static_cast<uint32_t>(textureType), shader->GetType());

	m_ConstantBuffer->Bind(shader->GetType());
}

void Material::Unbind(const Ref<Shader>& shader)
{
	for (auto [textureType, texture] : m_Textures)
		texture->Unbind(static_cast<uint32_t>(textureType), shader->GetType());
}
}
