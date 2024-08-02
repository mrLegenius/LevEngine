#include "levpch.h"
#include "D3D11RenderDevice.h"

#include "D3D11BlendState.h"
#include "D3D11ConstantBuffer.h"
#include "D3D11DepthStencilState.h"
#include "D3D11DispatchCommand.h"
#include "D3D11IndexBuffer.h"
#include "D3D11Query.h"
#include "D3D11RasterizerState.h"
#include "D3D11RenderCommands.h"
#include "D3D11RenderDebugEvent.h"
#include "D3D11RenderContext.h"
#include "D3D11RenderTarget.h"
#include "D3D11SamplerState.h"
#include "D3D11Shader.h"
#include "D3D11StructuredBuffer.h"
#include "D3D11Texture.h"
#include "D3D11VertexBuffer.h"

namespace LevEngine
{
    D3D11RenderDevice::D3D11RenderDevice()
    {
        LEV_PROFILE_FUNCTION();
        
        constexpr D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0
        };

        UINT createDeviceFlags = 0;
#ifdef LEV_DEBUG
        createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

        // This will be the feature level that 
        // is used to create our device and swap chain.
        D3D_FEATURE_LEVEL featureLevel;

        Microsoft::WRL::ComPtr<ID3D11Device> device;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;

        // First create a ID3D11Device and ID3D11DeviceContext
        HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE,
                                       nullptr, createDeviceFlags, featureLevels, _countof(featureLevels),
                                       D3D11_SDK_VERSION, &device, &featureLevel, &deviceContext);

        if (hr == E_INVALIDARG)
        {
            hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE,
                                   nullptr, createDeviceFlags, &featureLevels[1], _countof(featureLevels) - 1,
                                   D3D11_SDK_VERSION, &device, &featureLevel, &deviceContext);
        }

        if (FAILED(hr))
        {
            LEV_ASSERT("Failed to created DirectX 11 Device");
            return;
        }
        
        // Query for the ID3D11Device2 interface.
        if (FAILED(device.Get()->QueryInterface<ID3D11Device2>(&m_Device)))
        {
            LEV_ASSERT("Failed to create DirectX 11.2 device");
        }

        // Now get the immediate device context.
        m_Device->GetImmediateContext2(&m_DeviceContext);
    }

    D3D11RenderDevice::~D3D11RenderDevice()
    {
        if (m_DeviceContext)
            m_DeviceContext->Release();

        if (m_Device)
            m_Device->Release();
    }

    Microsoft::WRL::ComPtr<ID3D11Device2> D3D11RenderDevice::GetDevice() const
    {
        return m_Device;
    }

    Microsoft::WRL::ComPtr<ID3D11DeviceContext2> D3D11RenderDevice::GetDeviceContext() const
    {
        return m_DeviceContext;
    }

    Ref<ConstantBuffer> D3D11RenderDevice::CreateConstantBuffer(uint32_t size, uint32_t slot) const
    {
        return CreateRef<D3D11ConstantBuffer>(m_Device.Get(), size, slot);
    }

    Ref<BlendState> D3D11RenderDevice::CreateBlendState() const
    {
        return CreateRef<D3D11BlendState>(m_Device.Get());
    }

    Ref<DepthStencilState> D3D11RenderDevice::CreateDepthStencilState() const
    {
        return CreateRef<D3D11DepthStencilState>(m_Device.Get());
    }

    Ref<IndexBuffer> D3D11RenderDevice::CreateIndexBuffer(const uint32_t* indices, uint32_t count) const
    {
        return CreateRef<D3D11IndexBuffer>(m_Device.Get(), indices, count);
    }

    Ref<RasterizerState> D3D11RenderDevice::CreateRasterizerState() const
    {
        return CreateRef<D3D11RasterizerState>(m_Device.Get());
    }

    Ref<RenderTarget> D3D11RenderDevice::CreateRenderTarget() const
    {
        return CreateRef<D3D11RenderTarget>(m_Device.Get());
    }

    Ref<SamplerState> D3D11RenderDevice::CreateSamplerState() const
    {
        return CreateRef<D3D11SamplerState>(m_Device.Get());
    }

    Ref<StructuredBuffer> D3D11RenderDevice::CreateStructuredBuffer(const void* data, size_t count, uint32_t stride,
        CPUAccess cpuAccess, bool uav, UAVType uavType) const
    {
        return CreateRef<D3D11StructuredBuffer>(m_Device.Get(), data, count, stride, cpuAccess, uav, uavType);
    }

    Ref<VertexBuffer> D3D11RenderDevice::CreateVertexBuffer(const uint32_t size, const uint32_t stride) const
    {
        return CreateRef<D3D11VertexBuffer>(m_Device.Get(), size, stride);
    }

    Ref<VertexBuffer> D3D11RenderDevice::CreateVertexBuffer(const float* vertices, const uint32_t size,
        const uint32_t stride) const
    {
        return CreateRef<D3D11VertexBuffer>(m_Device.Get(), vertices, size, stride);
    }

    Ref<VertexBuffer> D3D11RenderDevice::CreateVertexBuffer(const int* vertices, const uint32_t size,
        const uint32_t stride) const
    {
        return CreateRef<D3D11VertexBuffer>(m_Device.Get(), vertices, size, stride);
    }

    Ref<Texture> D3D11RenderDevice::CreateTexture(const String& path, bool isLinear, bool generateMipMaps) const
    {
        return CreateRef<D3D11Texture>(m_Device.Get(), path, isLinear, generateMipMaps);
    }

    Ref<Texture> D3D11RenderDevice::CreateTexture2D(const uint16_t width, const uint16_t height, const uint16_t slices,
        const Texture::TextureFormat& format, void* data, const CPUAccess cpuAccess, const bool uav,
        const bool generateMipMaps) const
    {
        return D3D11Texture::CreateTexture2D(m_Device.Get(), width, height, slices, format, data, cpuAccess, uav, generateMipMaps);
    }

    Ref<Texture> D3D11RenderDevice::CreateTexture2D(const uint16_t width, const uint16_t height, const uint16_t slices,
        const Texture::TextureFormat& format, const CPUAccess cpuAccess, const bool uav,
        const bool generateMipMaps) const
    {
        return D3D11Texture::CreateTexture2D(m_Device.Get(), width, height, slices, format, cpuAccess, uav, generateMipMaps);
    }

    Ref<Texture> D3D11RenderDevice::CreateTextureCube(const uint16_t width, const uint16_t height,
        const Texture::TextureFormat& format, const CPUAccess cpuAccess, const bool uav,
        const bool generateMipMaps) const
    {
        return D3D11Texture::CreateTextureCube(m_Device.Get(), width, height, format, cpuAccess, uav, generateMipMaps);
    }

    Ref<Texture> D3D11RenderDevice::CreateTextureCube(const String paths[6], const bool isLinear) const
    {
        return CreateRef<D3D11Texture>(m_Device.Get(), paths, isLinear);
    }

    Ref<Query> D3D11RenderDevice::CreateQuery(Query::QueryType queryType, uint8_t numBuffers) const
    {
        return CreateRef<D3D11Query>(m_Device.Get(), queryType, numBuffers);
    }

    Ref<Shader> D3D11RenderDevice::CreateShader(const String& filepath, ShaderType shaderTypes) const
    {
        return CreateRef<D3D11Shader>(m_Device.Get(), filepath, shaderTypes);
    }

    Ref<RenderCommands> D3D11RenderDevice::CreateRenderCommands() const
    {
        return CreateRef<D3D11RenderCommands>(m_Device.Get());
    }

    Ref<DispatchCommands> D3D11RenderDevice::CreateDispatchCommands() const
    {
        return CreateRef<D3D11DispatchCommand>(m_Device.Get());
    }

    Ref<RenderDebugEvent> D3D11RenderDevice::CreateRenderDebugEvent() const
    {
        return CreateRef<D3D11RenderDebugEvent>(m_Device.Get());
    }

    Ref<RenderContext> D3D11RenderDevice::CreateRenderContext() const
    {
        return CreateRef<D3D11RenderContext>();
    }
}
