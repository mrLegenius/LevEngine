#pragma once
#include "Renderer/RendererAPI.h"

namespace LevEngine
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		void Init() override;

		void SetViewport(int x, int y, uint32_t width, uint32_t height) override;
		
		void SetClearColor(const glm::vec4& color) override;
		void Clear() override;
		
		void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
        virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override;

        virtual void SetLineWidth(float width) override;
	};
}
