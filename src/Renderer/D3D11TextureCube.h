#pragma once
#include "D3D11Texture.h"

class D3D11TextureCube : public Texture
{
public:
	D3D11TextureCube(const std::string paths[6]);
	~D3D11TextureCube() override;

	[[nodiscard]] uint16_t GetWidth() const override { return m_Width; }
	[[nodiscard]] uint16_t GetHeight() const override { return m_Height; }

	[[nodiscard]] bool IsLoaded() const override { return m_IsLoaded; }

	void Bind(uint32_t slot = 0) const override;
	void GenerateMipMaps() override;
	[[nodiscard]] Ref<Texture> GetSlice(unsigned slice) const override;
	[[nodiscard]] uint16_t GetDepth() const override;
	[[nodiscard]] bool IsTransparent() const override;
	void Resize(uint16_t width, uint16_t height, uint16_t depth) override;
	void Copy(Ref<Texture> other) override;
	uint8_t GetBPP() const override;
	void Clear(ClearFlags clearFlags, const Vector4& color, float depth, uint8_t stencil) override;
	void Unbind(uint32_t slot) const override;

private:
	std::string m_Paths[6];
	bool m_IsLoaded = false;

	ID3D11Texture2D* m_Texture;
	ID3D11ShaderResourceView* m_ShaderResourceView;
	ID3D11SamplerState* m_SamplerState;

	uint16_t m_Width;
	uint16_t m_Height;

	DXGI_FORMAT m_DataFormat;
};
