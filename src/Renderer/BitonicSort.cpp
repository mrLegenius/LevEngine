#include "pch.h"
#include "BitonicSort.h"

extern ID3D11DeviceContext* context;

BitonicSort::BitonicSort(const int numElements): m_NumElements(numElements)
{
	m_BitonicSortCS = ShaderAssets::BitonicSort();
	m_BitonicTransposeCS = ShaderAssets::BitonicTranspose();
	m_ConstantsBuffer = CreateRef<D3D11ConstantBuffer>(sizeof ConstantsGPUData, 0);
}

void BitonicSort::Sort(const Ref<D3D11StructuredBuffer> inBuffer, const Ref<D3D11StructuredBuffer> tempBuffer) const
{
	const uint32_t numElements = m_NumElements;
	constexpr uint32_t matrixWidth = BitonicBlockSize;
	const uint32_t matrixHeight = numElements / BitonicBlockSize;

	for (uint32_t level = 2; level <= BitonicBlockSize; level <<= 1) 
	{
		SetGPUSortConstants(level, level, matrixHeight, matrixWidth);

		// Sort the row data
		inBuffer->Bind(0, ShaderType::Compute, true, -1);
		m_BitonicSortCS->Bind();
		context->Dispatch(static_cast<int>(numElements / BitonicBlockSize), 1, 1);
	}

	inBuffer->Unbind(0, ShaderType::Compute, true);

	auto heightTransposeBlocks = static_cast<int>(matrixHeight / TransposeBlockSize);
	// Then sort the rows and columns for the levels > than the block size
	// Transpose. Sort the Columns. Transpose. Sort the Rows.
	for (uint32_t level = (BitonicBlockSize << 1); level <= numElements; level <<= 1) {
		// Transpose the data from buffer 1 into buffer 2
		SetGPUSortConstants(level / BitonicBlockSize, (level & ~numElements) / BitonicBlockSize, matrixWidth, matrixHeight);
		inBuffer->Bind(0, ShaderType::Compute, false);
		tempBuffer->Bind(0, ShaderType::Compute, true);

		m_BitonicTransposeCS->Bind();
		if (heightTransposeBlocks != 0)
			context->Dispatch(static_cast<int>(matrixWidth / TransposeBlockSize), heightTransposeBlocks, 1);
		inBuffer->Unbind(0, ShaderType::Compute, false);
		// Sort the transposed column data
		//tempBuffer->Bind(0, ShaderType::Compute, true);
		m_BitonicSortCS->Bind();
		context->Dispatch(static_cast<int>(numElements / BitonicBlockSize), 1, 1);
		tempBuffer->Unbind(0, ShaderType::Compute, true);

		// Transpose the data from buffer 2 back into buffer 1
		SetGPUSortConstants(BitonicBlockSize, level, matrixHeight, matrixWidth);

		tempBuffer->Bind(0, ShaderType::Compute, false);
		inBuffer->Bind(0, ShaderType::Compute, true, -1);

		m_BitonicTransposeCS->Bind();
		if (heightTransposeBlocks != 0)
			context->Dispatch(heightTransposeBlocks, static_cast<int>(matrixWidth / TransposeBlockSize), 1);
		tempBuffer->Unbind(0, ShaderType::Compute, false);
		// Sort the row data
		//inBuffer->Bind(0, ShaderType::Compute, true);
		m_BitonicSortCS->Bind();
		context->Dispatch(static_cast<int>(numElements / BitonicBlockSize), 1, 1);
		inBuffer->Unbind(0, ShaderType::Compute, true);
	}
}

void BitonicSort::SetGPUSortConstants(const uint32_t level, const uint32_t levelMask, const uint32_t width,
	const uint32_t height) const
{
	const ConstantsGPUData data {(int)level, (int)levelMask, (int)width, (int)height};
	m_ConstantsBuffer->SetData(&data);
}
