#include "pch.h"
#include "D3D11TextureCube.h"

#include <cassert>
#include <wrl/client.h>

#include "stb/include/stb_image.h"
namespace LevEngine
{
extern ID3D11DeviceContext* context;
extern Microsoft::WRL::ComPtr<ID3D11Device> device;

D3D11TextureCube::D3D11TextureCube(const std::string paths[6])
	: m_DataFormat(DXGI_FORMAT_UNKNOWN)
{
	int width, height, channels;

	std::copy_n(paths, 6, m_Paths);

	stbi_uc* data[6];

	for (int i = 0; i < 6; ++i)
		data[i] = stbi_load(paths[i].c_str(), &width, &height, &channels, 4);

	m_IsLoaded = true;
	m_Width = width;
	m_Height = height;

	if (channels == 3)
	{
		m_DataFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	}
	else if (channels == 4)
	{
		m_DataFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	}

	const int pitch = width * 4;

	assert((m_DataFormat != DXGI_FORMAT_UNKNOWN) && "Format is not supported!");

	// Texture

	D3D11_TEXTURE2D_DESC imageTextureDesc = {};

	imageTextureDesc.Width = width;
	imageTextureDesc.Height = height;
	imageTextureDesc.MipLevels = 1;
	imageTextureDesc.ArraySize = 6;
	imageTextureDesc.Format = m_DataFormat;
	imageTextureDesc.SampleDesc.Count = 1;
	imageTextureDesc.SampleDesc.Quality = 0;
	imageTextureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	imageTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	imageTextureDesc.CPUAccessFlags = 0;
	imageTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	D3D11_SUBRESOURCE_DATA imageSubresourceData[6];
	for (int i = 0; i < 6; ++i)
	{
		auto& subResourceData = imageSubresourceData[i];
		subResourceData.pSysMem = data[i];
		subResourceData.SysMemPitch = pitch;
		subResourceData.SysMemSlicePitch = 0;
	}

	auto result = device->CreateTexture2D(
		&imageTextureDesc,
		imageSubresourceData,
		&m_Texture
	);

	assert(SUCCEEDED(result));

	for (auto& i : data)
		stbi_image_free(i);

	//Shader resource view

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = m_DataFormat;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	result = device->CreateShaderResourceView(m_Texture,
		&srvDesc,
		&m_ShaderResourceView
	);

	assert(SUCCEEDED(result));

	// Sampler

	D3D11_SAMPLER_DESC ImageSamplerDesc = {};

	ImageSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	ImageSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	ImageSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	ImageSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	ImageSamplerDesc.MipLODBias = 0.0f;
	ImageSamplerDesc.MaxAnisotropy = 1;
	ImageSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	ImageSamplerDesc.BorderColor[0] = 1.0f;
	ImageSamplerDesc.BorderColor[1] = 1.0f;
	ImageSamplerDesc.BorderColor[2] = 1.0f;
	ImageSamplerDesc.BorderColor[3] = 1.0f;
	ImageSamplerDesc.MinLOD = -FLT_MAX;
	ImageSamplerDesc.MaxLOD = FLT_MAX;

	result = device->CreateSamplerState(&ImageSamplerDesc,
		&m_SamplerState);

	assert(SUCCEEDED(result));
}

D3D11TextureCube::~D3D11TextureCube()
{
	m_Texture->Release();
	m_ShaderResourceView->Release();
	m_SamplerState->Release();
}

void D3D11TextureCube::Bind(const uint32_t slot) const
{
	context->PSSetShaderResources(slot, 1, &m_ShaderResourceView);
	context->PSSetSamplers(slot, 1, &m_SamplerState);
}

void D3D11TextureCube::GenerateMipMaps()
{
}

Ref<Texture> D3D11TextureCube::GetSlice(unsigned slice) const
{
	return nullptr;
}

uint16_t D3D11TextureCube::GetDepth() const
{
	return 0;
}

bool D3D11TextureCube::IsTransparent() const
{
	return false;
}

void D3D11TextureCube::Resize(uint16_t width, uint16_t height, uint16_t depth)
{
}

void D3D11TextureCube::Copy(Ref<Texture> other)
{
}

uint8_t D3D11TextureCube::GetBPP() const
{
	return 0;
}

void D3D11TextureCube::Clear(ClearFlags clearFlags, const Vector4& color, float depth, uint8_t stencil)
{
}

void D3D11TextureCube::Unbind(uint32_t slot) const
{
	context->PSSetShaderResources(slot, 0, nullptr);
	context->PSSetSamplers(slot, 0, nullptr);
}
}