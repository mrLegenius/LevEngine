#include "OpenGLRendererAPI.h"

#include <glad/gl.h>

void LevEngine::OpenGLRendererAPI::Init()
{
	LEV_PROFILE_FUNCTION();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
}

void LevEngine::OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
{
	LEV_PROFILE_FUNCTION();
	
	glClearColor(color.r, color.g, color.b, color.a);
}

void LevEngine::OpenGLRendererAPI::Clear()
{
	LEV_PROFILE_FUNCTION();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void LevEngine::OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, const uint32_t indexCount)
{
	LEV_PROFILE_FUNCTION();

    vertexArray->Bind();
	const uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}

void LevEngine::OpenGLRendererAPI::SetViewport(const int x, const int y, const uint32_t width, const uint32_t height)
{
	LEV_PROFILE_FUNCTION();
	
	glViewport(x, y, width, height);
}
