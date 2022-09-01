#pragma once
#include "Renderer/Buffer.h"

namespace LevEngine
{
	class OpenGLVertexBuffer final : public VertexBuffer
	{
	public:
		explicit OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		~OpenGLVertexBuffer() override;

		void Bind() const override;
		void Unbind() const override;

		void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
		[[nodiscard]] const BufferLayout& GetLayout() const override { return m_Layout; }

        [[nodiscard]] const uint32_t GetSize() const override { return m_Size; };

		void SetData(const void* data, uint32_t size = 0) override;
	private:
		uint32_t m_RendererID = 0;
        uint32_t m_Size;
		BufferLayout m_Layout;
	};

	class OpenGLIndexBuffer final : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		~OpenGLIndexBuffer() override;

		void Bind() const override;
		void Unbind() const override;

		[[nodiscard]] uint32_t GetCount() const override { return m_Count; }

	private:
		uint32_t m_RendererID = 0;
		uint32_t m_Count = 0;
	};
}
