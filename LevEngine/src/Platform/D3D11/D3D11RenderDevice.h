#pragma once
#include <d3d11_2.h>
#include <wrl/client.h>

#include "Renderer/RenderDevice.h"

namespace LevEngine
{
    class D3D11RenderDevice : public RenderDevice
    {
    public:
        Microsoft::WRL::ComPtr<ID3D11Device2> GetDevice() const;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext2> GetDeviceContext() const;
        
        Ref<ConstantBuffer> CreateConstantBuffer(uint32_t size, uint32_t slot) const override;
        Ref<BlendState> CreateBlendState() const override;
        Ref<DepthStencilState> CreateDepthStencilState() const override;
        Ref<IndexBuffer> CreateIndexBuffer(const uint32_t* indices, uint32_t count) const override;
        Ref<RasterizerState> CreateRasterizerState() const override;
        Ref<RenderTarget> CreateRenderTarget() const override;
        Ref<SamplerState> CreateSamplerState() const override;
        
        Ref<StructuredBuffer> CreateStructuredBuffer(const void* data, size_t count, uint32_t stride,
            CPUAccess cpuAccess, bool uav, UAVType uavType) const override;
        
        Ref<VertexBuffer> CreateVertexBuffer(uint32_t size, uint32_t stride) const override;
        Ref<VertexBuffer> CreateVertexBuffer(const float* vertices, uint32_t size, uint32_t stride) const override;
        Ref<VertexBuffer> CreateVertexBuffer(const int* vertices, uint32_t size, uint32_t stride) const override;
        
        Ref<Texture> CreateTexture(const String& path, bool isLinear, bool generateMipMaps) const override;
        Ref<Texture> CreateTexture2D(uint16_t width, uint16_t height, uint16_t slices,
                                     const Texture::TextureFormat& format, void* data, CPUAccess cpuAccess, bool uav,
                                     bool generateMipMaps) const override;
        Ref<Texture> CreateTexture2D(uint16_t width, uint16_t height, uint16_t slices,
                                     const Texture::TextureFormat& format, CPUAccess cpuAccess, bool uav,
                                     bool generateMipMaps) const override;
        Ref<Texture> CreateTextureCube(uint16_t width, uint16_t height,
                                       const Texture::TextureFormat& format, CPUAccess cpuAccess, bool uav,
                                       bool generateMipMaps) const override;
        Ref<Texture> CreateTextureCube(const String paths[6], bool isLinear) const override;
        Ref<Query> CreateQuery(Query::QueryType queryType, uint8_t numBuffers) const override;
        Ref<Shader> CreateShader(const String& filepath, ShaderType shaderTypes) const override;
        Ref<RenderCommands> CreateRenderCommands() const override;
        Ref<DispatchCommands> CreateDispatchCommands() const override;
        Ref<RenderDebugEvent> CreateRenderDebugEvent() const override;
        Ref<RenderContext> CreateRenderContext() const override;

    private:

        Microsoft::WRL::ComPtr<ID3D11Device2> m_Device;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext2> m_DeviceContext;
    };
}
