#pragma once

namespace LevEngine
{
	class VertexBuffer
	{
	public:
		explicit VertexBuffer(const uint32_t size, const uint32_t stride) : m_Size(size), m_Stride(stride) { }
		virtual ~VertexBuffer() = default;

		static Ref<VertexBuffer> Create(uint32_t size, uint32_t stride);
		static Ref<VertexBuffer> Create(const float* vertices, uint32_t size, uint32_t stride);

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint32_t size = 0) = 0;

		[[nodiscard]] uint32_t GetSize() const { return m_Size; }

	protected:
		uint32_t m_Size;
		uint32_t m_Stride;
	};
}
