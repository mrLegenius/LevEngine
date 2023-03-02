#include "D3D11Texture.h"

#include <cassert>
#include <wrl/client.h>

#include "d3d11.h"
#include "../../external/stb/include/stb_image.h"

extern ID3D11DeviceContext* context;
extern Microsoft::WRL::ComPtr<ID3D11Device> device;

D3D11Texture2D::D3D11Texture2D(const std::string& path)
	: m_Path(path), m_InternalFormat(DXGI_FORMAT_UNKNOWN), m_DataFormat(DXGI_FORMAT_UNKNOWN)
{
	// Load image

	int width, height, channels;

	stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 4);

	if (!data) return;

	m_IsLoaded = true;
	m_Width = width;
	m_Height = height;

	if (channels == 3)
	{
		m_InternalFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		m_DataFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	}
	else if (channels == 4)
	{
		m_InternalFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		m_DataFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	}

	const int pitch = width * 4;

	assert(m_InternalFormat & m_DataFormat && "Format is not supported!");

	// Texture

	D3D11_TEXTURE2D_DESC imageTextureDesc = {};

	imageTextureDesc.Width = width;
	imageTextureDesc.Height = height;
	imageTextureDesc.MipLevels = 1;
	imageTextureDesc.ArraySize = 1;
	imageTextureDesc.Format = m_DataFormat;
	imageTextureDesc.SampleDesc.Count = 1;
	imageTextureDesc.SampleDesc.Quality = 0;
	imageTextureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	imageTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA imageSubresourceData = {};

	imageSubresourceData.pSysMem = data;
	imageSubresourceData.SysMemPitch = pitch;

	auto result = device->CreateTexture2D(
		&imageTextureDesc,
		&imageSubresourceData,
		&m_Texture
	);

	assert(SUCCEEDED(result));
	stbi_image_free(data);

	//Shader resource view
    
	result = device->CreateShaderResourceView(m_Texture,
		nullptr,
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

D3D11Texture2D::~D3D11Texture2D()
{
	m_Texture->Release();
}

void D3D11Texture2D::Bind(const uint32_t slot) const
{
	context->PSSetShaderResources(slot, 1, &m_ShaderResourceView);
	context->PSSetSamplers(slot, 1, &m_SamplerState);
}