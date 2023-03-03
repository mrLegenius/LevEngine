#pragma once
#include <cstdint>
#include <dxgiformat.h>
#include <string>

#include "D3D11ConstantBuffer.h"

class Texture
{
public:
	virtual ~Texture() = default;
	[[nodiscard]] virtual uint32_t GetWidth() const = 0;
	[[nodiscard]] virtual uint32_t GetHeight() const = 0;

	virtual void Bind(uint32_t slot = 0) const = 0;

	[[nodiscard]] virtual bool IsLoaded() const = 0;
};

class D3D11Texture2D : public Texture
{
public:
	D3D11Texture2D(const std::string& path);
	~D3D11Texture2D() override;

	[[nodiscard]] uint32_t GetWidth() const override { return m_Width; }
	[[nodiscard]] uint32_t GetHeight() const override { return m_Height; }
	[[nodiscard]] const std::string& GetPath() const { return m_Path; }

	[[nodiscard]] bool IsLoaded() const override { return m_IsLoaded; }

	void Bind(uint32_t slot = 0) const override;
private:
	std::string m_Path;
	bool m_IsLoaded = false;

	ID3D11Texture2D* m_Texture;
	ID3D11ShaderResourceView* m_ShaderResourceView;
	ID3D11SamplerState* m_SamplerState;

	uint32_t m_Width;
	uint32_t m_Height;

	DXGI_FORMAT m_DataFormat;
};
