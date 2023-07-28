#pragma once
#include "Buffer.h"

namespace LevEngine
{
	class VertexBuffer
	{
	public:
		explicit VertexBuffer(const uint32_t size) : m_Size(size) { }
		virtual ~VertexBuffer() = default;

		static Ref<VertexBuffer> Create(uint32_t size);
		static Ref<VertexBuffer> Create(const float* vertices, uint32_t size);

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint32_t size = 0) = 0;

		void SetLayout(const BufferLayout& layout) { m_Layout = layout; }
		[[nodiscard]] const BufferLayout& GetLayout() const { return m_Layout; }

		[[nodiscard]] uint32_t GetSize() const { return m_Size; }

	protected:
		uint32_t m_Size;
		BufferLayout m_Layout;
	};
}
