#include "levpch.h"
#include "Material.h"

namespace LevEngine
{
Material::Material()
{
	m_ConstantBuffer = ConstantBuffer::Create(sizeof GPUData);

	m_Textures[TextureType::Emissive] = TextureLibrary::GetEmptyTexture();
	m_Textures[TextureType::Diffuse] = TextureLibrary::GetEmptyTexture();
	m_Textures[TextureType::Specular] = TextureLibrary::GetEmptyTexture();
	m_Textures[TextureType::Normal] = TextureLibrary::GetEmptyTexture();
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

	auto parameter = shader->GetShaderParameterByName("MaterialConstantBuffer");

	if (parameter.IsValid())
	{
		parameter.Set(m_ConstantBuffer);
		parameter.Bind();
	}
}

void Material::Unbind(const Ref<Shader>& shader)
{
	for (auto [textureType, texture] : m_Textures)
	{
		if (texture)
			texture->Unbind(static_cast<uint32_t>(textureType), shader->GetType());
	}

	const auto parameter = shader->GetShaderParameterByName("MaterialConstantBuffer");
	if (parameter.IsValid())
		parameter.Unbind();
}
}
