#pragma once
#include <dxgiformat.h>
#include <d3d11.h>

#include "Renderer/ClearFlags.h"
#include "Renderer/CPUAccess.h"
#include "Renderer/Texture.h"

namespace LevEngine
{
class D3D11Texture : public Texture
{
public:
	D3D11Texture() = default;
	static Ref<D3D11Texture> CreateTexture2D(uint16_t width, uint16_t height, uint16_t slices, const TextureFormat& format, CPUAccess cpuAccess = CPUAccess::None, bool uav = false);
	explicit D3D11Texture(const std::string& path);
	explicit D3D11Texture(const std::string paths[6]);
	~D3D11Texture() override;

	[[nodiscard]] uint16_t GetWidth() const override { return m_Width; }
	[[nodiscard]] uint16_t GetHeight() const override { return m_Height; }

	[[nodiscard]] bool IsLoaded() const override { return m_IsLoaded; }

	void Bind(uint32_t slot, Shader::Type type) const override;
	void Unbind(uint32_t slot, Shader::Type type) const override;
	void Clear(ClearFlags clearFlags, const Vector4& color, float depth, uint8_t stencil) override;

	ID3D11Resource* GetTextureResource() const;
	void Resize2D(uint16_t width, uint16_t height);
	ID3D11ShaderResourceView* GetShaderResourceView() const { return m_ShaderResourceView; }
	ID3D11RenderTargetView* GetRenderTargetView() const { return m_RenderTargetView; }
	ID3D11DepthStencilView* GetDepthStencilView() const { return m_DepthStencilView; }
	ID3D11UnorderedAccessView* GetUnorderedAccessView() const { return m_UnorderedAccessView; }

	[[nodiscard]] Ref<Texture> GetSlice(unsigned slice) const override { return nullptr; }
	[[nodiscard]] uint16_t GetDepth() const override { return m_NumSlices; }
	[[nodiscard]] bool IsTransparent() const override { return m_IsTransparent; }
	void Resize(uint16_t width, uint16_t height = 0, uint16_t depth = 0) override;
	void CopyFrom(Ref<Texture> sourceTexture) override;
	Ref<Texture> Clone() override;
	uint8_t GetBPP() const override { return m_BPP; }

	void GenerateMipMaps() override;
	[[nodiscard]] void* GetId() override { return m_ShaderResourceView; }

protected:

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

	bool m_IsLoaded = false;

	Dimension m_TextureDimension{};

	bool m_IsDirty = false;
};
}