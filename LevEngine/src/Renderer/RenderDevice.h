#pragma once
#include "Pipeline/Texture.h"
#include "Query/Query.h"
#include "Shader/ShaderMacros.h"

namespace LevEngine
{
    class RenderContext;
    class StructuredBuffer;
    class VertexBuffer;
    class IndexBuffer;
    class DepthStencilState;
    class DispatchCommands;
    class RenderDebugEvent;
    class RenderCommands;
    enum class UAVType;
    enum class CPUAccess;
    class SamplerState;
    class RenderTarget;
    class RasterizerState;
    class BlendState;
    class ConstantBuffer;
    class Shader;
    class Texture;
    class Query;
    
    class RenderDevice
    {
    public:
        virtual ~RenderDevice() = default;

        virtual Ref<ConstantBuffer> CreateConstantBuffer(uint32_t size, uint32_t slot = 0) const = 0;
        virtual Ref<BlendState> CreateBlendState() const = 0;
        virtual Ref<DepthStencilState> CreateDepthStencilState() const = 0;
        virtual Ref<IndexBuffer> CreateIndexBuffer(const uint32_t* indices, uint32_t count) const = 0;
        virtual Ref<RasterizerState> CreateRasterizerState() const = 0;
        virtual Ref<RenderTarget> CreateRenderTarget() const = 0;
        virtual Ref<SamplerState> CreateSamplerState() const = 0;
        
        virtual Ref<VertexBuffer> CreateVertexBuffer(uint32_t size, uint32_t stride) const = 0;
        virtual Ref<VertexBuffer> CreateVertexBuffer(const float* vertices, uint32_t size, uint32_t stride) const = 0;
        virtual Ref<VertexBuffer> CreateVertexBuffer(const int* vertices, uint32_t size, uint32_t stride) const = 0;
        
        virtual Ref<StructuredBuffer> CreateStructuredBuffer(
            const void* data,
            size_t count,
            uint32_t stride, 
            CPUAccess cpuAccess, 
            bool uav, 
            UAVType uavType) const = 0;
        
        virtual Ref<Texture> CreateTexture(const String& path,
                                           bool isLinear,
                                           bool generateMipMaps) const = 0;
        
        virtual Ref<Texture> CreateTexture2D(uint16_t width, uint16_t height, uint16_t slices,
                                             const Texture::TextureFormat& format,
                                             void* data,
                                             CPUAccess cpuAccess,
                                             bool uav,
                                             bool generateMipMaps) const = 0;
        virtual Ref<Texture> CreateTexture2D(uint16_t width, uint16_t height, uint16_t slices,
                                             const Texture::TextureFormat& format,
                                             CPUAccess cpuAccess,
                                             bool uav,
                                             bool generateMipMaps) const = 0;

        virtual Ref<Texture> CreateTextureCube(uint16_t width, uint16_t height,
                                               const Texture::TextureFormat& format,
                                               CPUAccess cpuAccess,
                                               bool uav,
                                               bool generateMipMaps) const = 0;
        virtual Ref<Texture> CreateTextureCube(const String paths[6], bool isLinear) const = 0;
        
        virtual Ref<Query> CreateQuery(Query::QueryType queryType, uint8_t numBuffers) const = 0;
        virtual Ref<Shader> CreateShader(const String& filepath, ShaderType shaderTypes, const ShaderMacros& macros) const = 0;
        virtual Ref<RenderCommands> CreateRenderCommands() const = 0;
        virtual Ref<DispatchCommands> CreateDispatchCommands() const = 0;
        virtual Ref<RenderDebugEvent> CreateRenderDebugEvent() const = 0;
        virtual Ref<RenderContext> CreateRenderContext() const = 0;
    };
}

