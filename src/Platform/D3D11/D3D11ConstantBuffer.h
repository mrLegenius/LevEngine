#pragma once
#include <d3d11.h>

#include "Renderer/ConstantBuffer.h"

namespace LevEngine
{
class D3D11ConstantBuffer final : public ConstantBuffer
{
public:
	explicit D3D11ConstantBuffer(uint32_t size, uint32_t slot = 0);
	~D3D11ConstantBuffer() override;

	void SetData(const void* data, uint32_t size = 0) const override;
private:
	ID3D11Buffer* m_Buffer = nullptr;
};
}
