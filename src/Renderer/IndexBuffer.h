#pragma once
namespace LevEngine
{
	class IndexBuffer
	{
	public:
		explicit IndexBuffer(const uint32_t count) : m_Count(count) { }
		virtual ~IndexBuffer() = default;

		static Ref<IndexBuffer> Create(const uint32_t* indices, uint32_t count);

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		[[nodiscard]] uint32_t GetCount() const { return m_Count; }
	protected:
		uint32_t m_Count = 0;
	};
}
