#pragma once
#include "D3D11Texture.h"

class D3D11TextureCube : public Texture
{
public:
	D3D11TextureCube(const std::string paths[6]);
	~D3D11TextureCube() override;

	[[nodiscard]] uint32_t GetWidth() const override { return m_Width; }
	[[nodiscard]] uint32_t GetHeight() const override { return m_Height; }

	[[nodiscard]] bool IsLoaded() const override { return m_IsLoaded; }

	void Bind(uint32_t slot = 0) const override;
private:
	std::string m_Paths[6];
	bool m_IsLoaded = false;

	ID3D11Texture2D* m_Texture;
	ID3D11ShaderResourceView* m_ShaderResourceView;
	ID3D11SamplerState* m_SamplerState;

	uint32_t m_Width;
	uint32_t m_Height;

	DXGI_FORMAT m_DataFormat;
};
