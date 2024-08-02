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
#include "D3D11RendererContext.h"
#include "D3D11RenderTarget.h"
#include "D3D11SamplerState.h"
#include "D3D11Shader.h"
#include "D3D11StructuredBuffer.h"
#include "D3D11Texture.h"
#include "D3D11VertexBuffer.h"

namespace LevEngine
{
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
        return CreateRef<D3D11ConstantBuffer>(size, slot);
    }

    Ref<BlendState> D3D11RenderDevice::CreateBlendState() const
    {
        return CreateRef<D3D11BlendState>();
    }

    Ref<DepthStencilState> D3D11RenderDevice::CreateDepthStencilState() const
    {
        return CreateRef<D3D11DepthStencilState>();
    }

    Ref<IndexBuffer> D3D11RenderDevice::CreateIndexBuffer(const uint32_t* indices, uint32_t count) const
    {
        return CreateRef<D3D11IndexBuffer>(indices, count);
    }

    Ref<RasterizerState> D3D11RenderDevice::CreateRasterizerState() const
    {
        return CreateRef<D3D11RasterizerState>();
    }

    Ref<RenderTarget> D3D11RenderDevice::CreateRenderTarget() const
    {
        return CreateRef<D3D11RenderTarget>();
    }

    Ref<SamplerState> D3D11RenderDevice::CreateSamplerState() const
    {
        return CreateRef<D3D11SamplerState>();
    }

    Ref<StructuredBuffer> D3D11RenderDevice::CreateStructuredBuffer(const void* data, size_t count, uint32_t stride,
        CPUAccess cpuAccess, bool uav, UAVType uavType) const
    {
        return CreateRef<D3D11StructuredBuffer>(data, count, stride, cpuAccess, uav, uavType);
    }

    Ref<VertexBuffer> D3D11RenderDevice::CreateVertexBuffer(const uint32_t size, const uint32_t stride) const
    {
        return CreateRef<D3D11VertexBuffer>(size, stride);
    }

    Ref<VertexBuffer> D3D11RenderDevice::CreateVertexBuffer(const float* vertices, const uint32_t size,
        const uint32_t stride) const
    {
        return CreateRef<D3D11VertexBuffer>(vertices, size, stride);
    }

    Ref<VertexBuffer> D3D11RenderDevice::CreateVertexBuffer(const int* vertices, const uint32_t size,
        const uint32_t stride) const
    {
        return CreateRef<D3D11VertexBuffer>(vertices, size, stride);
    }

    Ref<Texture> D3D11RenderDevice::CreateTexture(const String& path, bool isLinear, bool generateMipMaps) const
    {
        return CreateRef<D3D11Texture>(path, isLinear, generateMipMaps);
    }

    Ref<Texture> D3D11RenderDevice::CreateTexture2D(const uint16_t width, const uint16_t height, const uint16_t slices,
        const Texture::TextureFormat& format, void* data, const CPUAccess cpuAccess, const bool uav,
        const bool generateMipMaps) const
    {
        return D3D11Texture::CreateTexture2D(width, height, slices, format, data, cpuAccess, uav, generateMipMaps);
    }

    Ref<Texture> D3D11RenderDevice::CreateTexture2D(const uint16_t width, const uint16_t height, const uint16_t slices,
        const Texture::TextureFormat& format, const CPUAccess cpuAccess, const bool uav,
        const bool generateMipMaps) const
    {
        return D3D11Texture::CreateTexture2D(width, height, slices, format, cpuAccess, uav, generateMipMaps);
    }

    Ref<Texture> D3D11RenderDevice::CreateTextureCube(const uint16_t width, const uint16_t height,
        const Texture::TextureFormat& format, const CPUAccess cpuAccess, const bool uav,
        const bool generateMipMaps) const
    {
        return D3D11Texture::CreateTextureCube(width, height, format, cpuAccess, uav, generateMipMaps);
    }

    Ref<Texture> D3D11RenderDevice::CreateTextureCube(const String paths[6], const bool isLinear) const
    {
        return CreateRef<D3D11Texture>(paths, isLinear);
    }

    Ref<Query> D3D11RenderDevice::CreateQuery(Query::QueryType queryType, uint8_t numBuffers) const
    {
        return CreateRef<D3D11Query>(queryType, numBuffers);
    }

    Ref<Shader> D3D11RenderDevice::CreateShader(const String& filepath, ShaderType shaderTypes) const
    {
        return CreateRef<D3D11Shader>(filepath, shaderTypes);
    }

    Ref<RenderCommands> D3D11RenderDevice::CreateRenderCommands() const
    {
        return CreateRef<D3D11RenderCommands>();
    }

    Ref<DispatchCommands> D3D11RenderDevice::CreateDispatchCommands() const
    {
        return CreateRef<D3D11DispatchCommand>();
    }

    Ref<RenderDebugEvent> D3D11RenderDevice::CreateRenderDebugEvent() const
    {
        return CreateRef<D3D11RenderDebugEvent>();
    }

    Ref<RenderContext> D3D11RenderDevice::CreateRenderContext() const
    {
        return CreateRef<D3D11RendererContext>();
    }
}
