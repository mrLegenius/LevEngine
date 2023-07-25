#pragma once
#include <cstdint>
#include <cstdint>
#include <dxgiformat.h>
#include <SimpleMath.h>
#include <string>
#include <d3d11.h>

#include "ClearFlags.h"
#include "CPUAccess.h"
#include "Kernel/PointerUtils.h"

using namespace DirectX::SimpleMath;
namespace LevEngine
{
class Texture
{
public:
	enum class Dimension
	{
		Texture1D,
		Texture1DArray,
		Texture2D,
		Texture2DArray,
		Texture3D,
		TextureCube,
	};
	// The number of components used to create the texture.
	enum class Components
	{
		R,              // One red component.
		RG,             // Red, and green components.
		RGB,            // Red, green, and blue components.
		RGBA,           // Red, green, blue, and alpha components.
		Depth,          // Depth component.
		DepthStencil    // Depth and stencil in the same texture.
	};

	// The type of components in the texture.
	enum class Type
	{
		Typeless,           // Typeless formats.
		// TODO: sRGB type
		UnsignedNormalized, // Unsigned normalized (8, 10, or 16-bit unsigned integer values mapped to the range [0..1])
		SignedNormalized,   // Signed normalized (8, or 16-bit signed integer values mapped to the range [-1..1])
		Float,              // Floating point format (16, or 32-bit).
		UnsignedInteger,    // Unsigned integer format (8, 16, or 32-bit unsigned integer formats).
		SignedInteger,      // Signed integer format (8, 16, or 32-bit signed integer formats).
	};

	struct TextureFormat
	{
		Components Components;
		Type Type;

		// For multi-sample textures, we can specify how many samples we want 
		// to use for this texture. Valid values are usually in the range [1 .. 16]
		// depending on hardware support.
		// A value of 1 will effectively disable multisampling in the texture.
		uint8_t NumSamples;

		// Components should commonly be 8, 16, or 32-bits but some texture formats
		// support 1, 10, 11, 12, or 24-bits per component.
		uint8_t RedBits;
		uint8_t GreenBits;
		uint8_t BlueBits;
		uint8_t AlphaBits;
		uint8_t DepthBits;
		uint8_t StencilBits;

		// By default create a 4-component unsigned normalized texture with 8-bits per component and no multisampling.
		TextureFormat(Texture::Components components = Components::RGBA,
			Texture::Type type = Type::UnsignedNormalized,
			uint8_t numSamples = 1,
			uint8_t redBits = 8,
			uint8_t greenBits = 8,
			uint8_t blueBits = 8,
			uint8_t alphaBits = 8,
			uint8_t depthBits = 0,
			uint8_t stencilBits = 0)
			: Components(components)
			, Type(type)
			, NumSamples(numSamples)
			, RedBits(redBits)
			, GreenBits(greenBits)
			, BlueBits(blueBits)
			, AlphaBits(alphaBits)
			, DepthBits(depthBits)
			, StencilBits(stencilBits)
		{}
	};

	// For cube maps, we may need to access a particular face of the cube map.
	enum class CubeFace
	{
		Right,  // +X
		Left,   // -X
		Top,    // +Y
		Bottom, // -Y
		Front,  // +Z
		Back,   // -Z
	};

	virtual ~Texture() = default;

	virtual void GenerateMipMaps() = 0;

	[[nodiscard]] virtual Ref<Texture> GetSlice(unsigned int slice) const = 0;

	[[nodiscard]] virtual uint16_t GetWidth() const = 0;
	[[nodiscard]] virtual uint16_t GetHeight() const = 0;
	[[nodiscard]] virtual uint16_t GetDepth() const = 0;

	[[nodiscard]] virtual bool IsTransparent() const = 0;

	virtual void Resize(uint16_t width, uint16_t height = 0, uint16_t depth = 0) = 0;

	virtual void Copy(Ref<Texture> other) = 0;

	virtual uint8_t GetBPP() const = 0;

	virtual void Clear(ClearFlags clearFlags = ClearFlags::All, const Vector4& color = Vector4::Zero, float depth = 1.0f, uint8_t stencil = 0) = 0;

	virtual void Bind(uint32_t slot = 0) const = 0;
	virtual void Unbind(uint32_t slot = 0) const = 0;

	[[nodiscard]] virtual bool IsLoaded() const = 0;
};

class D3D11Texture : public Texture
{
public:
	D3D11Texture() = default;
	static Ref<D3D11Texture> CreateTexture2D(uint16_t width, uint16_t height, uint16_t slices, const TextureFormat& format, CPUAccess cpuAccess = CPUAccess::None, bool uav = false);
	D3D11Texture(const std::string& path);
	~D3D11Texture() override;

	[[nodiscard]] uint16_t GetWidth() const override { return m_Width; }
	[[nodiscard]] uint16_t GetHeight() const override { return m_Height; }
	[[nodiscard]] const std::string& GetPath() const { return m_Path; }

	[[nodiscard]] bool IsLoaded() const override { return m_IsLoaded; }

	void Bind(uint32_t slot = 0) const override;
	void Clear(ClearFlags clearFlags, const Vector4& color, float depth, uint8_t stencil) override;

	ID3D11Resource* GetTextureResource() const;
	void Resize2D(uint16_t width, uint16_t height);
	ID3D11ShaderResourceView* GetShaderResourceView() const { return m_ShaderResourceView; }
	ID3D11RenderTargetView* GetRenderTargetView() const { return m_RenderTargetView; }
	ID3D11DepthStencilView* GetDepthStencilView() const { return m_DepthStencilView; }
	ID3D11UnorderedAccessView* GetUnorderedAccessView() const { return m_UnorderedAccessView; }
	ID3D11SamplerState* GetSamplerState() const { return m_SamplerState; }

	[[nodiscard]] Ref<Texture> GetSlice(unsigned slice) const override { return nullptr; }
	[[nodiscard]] uint16_t GetDepth() const override { return m_NumSlices; }
	[[nodiscard]] bool IsTransparent() const override { return m_IsTransparent; }
	void Resize(uint16_t width, uint16_t height = 0, uint16_t depth = 0) override;
	void Copy(Ref<Texture> other) override;
	uint8_t GetBPP() const override { return m_BPP; }
	void Unbind(uint32_t slot) const override;
	void GenerateMipMaps() override;

protected:

	ID3D11SamplerState* m_SamplerState = nullptr;

	ID3D11Texture1D* m_Texture1D = nullptr;
	ID3D11Texture2D* m_Texture2D = nullptr;
	ID3D11Texture3D* m_Texture3D = nullptr;

	// Use this to map the texture to a shader for reading.
	ID3D11ShaderResourceView* m_ShaderResourceView = nullptr;
	// Use this to map the texture to a render target for writing.
	ID3D11RenderTargetView* m_RenderTargetView = nullptr;
	// Use this texture as the depth/stencil buffer of a render target.
	ID3D11DepthStencilView* m_DepthStencilView = nullptr;
	// Use this texture as a Unordered Acccess View (RWTexture)
	ID3D11UnorderedAccessView* m_UnorderedAccessView = nullptr;

	uint16_t m_Width = 0;
	uint16_t m_Height = 0;
	uint16_t m_NumSlices = 1;

	TextureFormat m_TextureFormat{};

	// DXGI texture format support flags
	UINT m_TextureResourceFormatSupport{};
	UINT m_DepthStencilViewFormatSupport{};
	UINT m_ShaderResourceViewFormatSupport;
	UINT m_RenderTargetViewFormatSupport{};
	UINT m_UnorderedAccessViewFormatSupport{};

	CPUAccess m_CPUAccess = CPUAccess::None;
	// Set to true if CPU write access is supported.
	bool m_Dynamic = false;
	// This resource should be used as a UAV.
	bool m_UAV = false;

	DXGI_FORMAT m_TextureResourceFormat = DXGI_FORMAT_UNKNOWN;
	DXGI_FORMAT m_DepthStencilViewFormat = DXGI_FORMAT_UNKNOWN;
	DXGI_FORMAT m_RenderTargetViewFormat = DXGI_FORMAT_UNKNOWN;
	DXGI_FORMAT m_ShaderResourceViewFormat = DXGI_FORMAT_UNKNOWN;
	DXGI_FORMAT m_UnorderedAccessViewFormat = DXGI_FORMAT_UNKNOWN;

	DXGI_SAMPLE_DESC m_SampleDesc{};

	bool m_GenerateMipMaps = false;

	uint8_t m_BPP{};

	uint16_t m_Pitch{};

	bool m_IsTransparent{};

	typedef std::vector<uint8_t> ColorBuffer;
	ColorBuffer m_Buffer{};

	std::string m_Path{};
	bool m_IsLoaded = false;

	Dimension m_TextureDimension{};

	bool m_IsDirty = false;
};
}