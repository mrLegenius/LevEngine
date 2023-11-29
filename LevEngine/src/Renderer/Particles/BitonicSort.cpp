#include "levpch.h"

#include "BitonicSort.h"

#include "Renderer/ConstantBuffer.h"
#include "Renderer/Shader.h"
#include "Assets/EngineAssets.h"
#include "Renderer/DispatchCommand.h"
#include "Renderer/StructuredBuffer.h"

namespace LevEngine
{
	
BitonicSort::BitonicSort(const int numElements): m_NumElements(numElements)
{
	m_BitonicSortCS = ShaderAssets::BitonicSort();
	m_BitonicTransposeCS = ShaderAssets::BitonicTranspose();
	m_ConstantsBuffer = ConstantBuffer::Create(sizeof ConstantsGPUData, 0);
}

void BitonicSort::Sort(const Ref<StructuredBuffer>& inBuffer, const Ref<StructuredBuffer>& tempBuffer) const
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
		DispatchCommand::Dispatch(numElements / BitonicBlockSize, 1, 1);
	}

	inBuffer->Unbind(0, ShaderType::Compute, true);

	const auto heightTransposeBlocks = static_cast<int>(matrixHeight / TransposeBlockSize);
	// Then sort the rows and columns for the levels > than the block size
	// Transpose. Sort the Columns. Transpose. Sort the Rows.
	for (uint32_t level = BitonicBlockSize << 1; level <= numElements; level <<= 1) {
		// Transpose the data from buffer 1 into buffer 2
		SetGPUSortConstants(level / BitonicBlockSize, (level & ~numElements) / BitonicBlockSize, matrixWidth, matrixHeight);
		inBuffer->Bind(0, ShaderType::Compute, false);
		tempBuffer->Bind(0, ShaderType::Compute, true);

		m_BitonicTransposeCS->Bind();
		if (heightTransposeBlocks != 0)
			DispatchCommand::Dispatch(matrixWidth / TransposeBlockSize, heightTransposeBlocks, 1);
		inBuffer->Unbind(0, ShaderType::Compute, false);
		// Sort the transposed column data
		//tempBuffer->Bind(0, Shader::Type::Compute, true);
		m_BitonicSortCS->Bind();
		DispatchCommand::Dispatch(numElements / BitonicBlockSize, 1, 1);
		tempBuffer->Unbind(0, ShaderType::Compute, true);

		// Transpose the data from buffer 2 back into buffer 1
		SetGPUSortConstants(BitonicBlockSize, level, matrixHeight, matrixWidth);

		tempBuffer->Bind(0, ShaderType::Compute, false);
		inBuffer->Bind(0, ShaderType::Compute, true, -1);

		m_BitonicTransposeCS->Bind();
		if (heightTransposeBlocks != 0)
			DispatchCommand::Dispatch(heightTransposeBlocks, matrixWidth / TransposeBlockSize, 1);
		tempBuffer->Unbind(0, ShaderType::Compute, false);
		// Sort the row data
		//inBuffer->Bind(0, Shader::Type::Compute, true);
		m_BitonicSortCS->Bind();
		DispatchCommand::Dispatch(numElements / BitonicBlockSize, 1, 1);
		inBuffer->Unbind(0, ShaderType::Compute, true);
	}
}

void BitonicSort::SetGPUSortConstants(const uint32_t level, const uint32_t levelMask, const uint32_t width,
	const uint32_t height) const
{
	const ConstantsGPUData data {
		static_cast<int>(level),
		static_cast<int>(levelMask),
		static_cast<int>(width),
		static_cast<int>(height)};
	
	m_ConstantsBuffer->SetData(&data);
	m_ConstantsBuffer->Bind(ShaderType::Compute);
}
}
