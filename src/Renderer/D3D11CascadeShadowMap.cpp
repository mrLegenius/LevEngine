#include <cassert>
#include <wrl/client.h>

#include "D3D11CascadeShadowMap.h"

extern ID3D11DeviceContext* context;
extern Microsoft::WRL::ComPtr<ID3D11Device> device;

D3D11CascadeShadowMap::D3D11CascadeShadowMap(uint32_t width, uint32_t height) : m_Width(width), m_Height(height)
{
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

    desc.Format = DXGI_FORMAT_R32_TYPELESS;
    desc.MipLevels = 1;
    desc.ArraySize = RenderSettings::CascadeCount;
    desc.SampleDesc.Count = 1;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
    desc.Height = width;
    desc.Width = height;

    HRESULT hr = device->CreateTexture2D(
        &desc,
        nullptr,
        &m_Texture
    );

    assert(SUCCEEDED(hr) && "Can't create a Texture2D for DepthBuffer");


    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    shaderResourceViewDesc.Texture2DArray = { 0, 1, 0, RenderSettings::CascadeCount };

    hr = device->CreateShaderResourceView(
        m_Texture,
        &shaderResourceViewDesc,
        &m_ShaderResourceView
    );

    assert(SUCCEEDED(hr) && "Can't create a ShaderResourceView for DepthBuffer");

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
    depthStencilViewDesc.Texture2DArray = { 0, 0, RenderSettings::CascadeCount };

    hr = device->CreateDepthStencilView(
        m_Texture,
        &depthStencilViewDesc,
        &m_DepthStencilView
    );

    assert(SUCCEEDED(hr) && "Can't create a DepthStencilView for DepthBuffer");

    D3D11_SAMPLER_DESC comparisonSamplerDesc;
    ZeroMemory(&comparisonSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
    comparisonSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    comparisonSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    comparisonSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    comparisonSamplerDesc.BorderColor[0] = 1.0f;
    comparisonSamplerDesc.BorderColor[1] = 1.0f;
    comparisonSamplerDesc.BorderColor[2] = 1.0f;
    comparisonSamplerDesc.BorderColor[3] = 1.0f;
    comparisonSamplerDesc.MinLOD = 0.f;
    comparisonSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    comparisonSamplerDesc.MipLODBias = 0.f;
    comparisonSamplerDesc.MaxAnisotropy = 0;
    comparisonSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    comparisonSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;

    // Point filtered shadows can be faster, and may be a good choice when
	// rendering on hardware with lower feature levels.

    hr = device->CreateSamplerState(
        &comparisonSamplerDesc,
        &m_SamplerState);

    assert(SUCCEEDED(hr) && "Can't create a SamplerState for DepthBuffer");

    CD3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.FillMode = D3D11_FILL_SOLID;
    //rastDesc.FillMode = D3D11_FILL_WIREFRAME;

    auto res = device->CreateRasterizerState(&rastDesc, &m_RastState);

    assert(SUCCEEDED(hr) && "Can't create a RasterizerState for DepthBuffer");
}

D3D11CascadeShadowMap::~D3D11CascadeShadowMap()
{
    m_Texture->Release();
    m_ShaderResourceView->Release();
	m_DepthStencilView->Release();

    m_SamplerState->Release();
}

void D3D11CascadeShadowMap::Bind(uint32_t slot) const
{
    context->PSSetShaderResources(slot, 1, &m_ShaderResourceView);
    context->PSSetSamplers(slot, 1, &m_SamplerState);
}

void D3D11CascadeShadowMap::SetRenderTarget() const
{
    context->RSSetState(m_RastState);
    context->OMSetRenderTargets(
        0,
        nullptr,
        m_DepthStencilView
    );
    context->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
}