#include "OpenGLRendererAPI.h"

#include <glad/gl.h>

namespace LevEngine
{
    static GLenum ConvertDepthFuncToGl(DepthFunc depthFunc)
    {
        switch (depthFunc)
        {
            case DepthFunc::Never:
                return GL_NEVER;
            case DepthFunc::Less:
                return GL_LESS;
            case DepthFunc::LessOrEqual:
                return GL_LEQUAL;
            case DepthFunc::Equal:
                return GL_EQUAL;
            case DepthFunc::NotEqual:
                return GL_NOTEQUAL;
            case DepthFunc::GreaterOrEqual:
                return GL_GEQUAL;
            case DepthFunc::Greater:
                return GL_GREATER;
            case DepthFunc::Always:
                return GL_ALWAYS;
            default:
            LEV_ASSERT(false, "[OpenGL Renderer API] Unknown depth function");
        }
    }

    void OpenGLRendererAPI::Init()
    {
        LEV_PROFILE_FUNCTION();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LINE_SMOOTH);
    }

    void OpenGLRendererAPI::SetClearColor(const glm::vec4 &color)
    {
        LEV_PROFILE_FUNCTION();

        glClearColor(color.r, color.g, color.b, color.a);
    }

    void OpenGLRendererAPI::Clear()
    {
        LEV_PROFILE_FUNCTION();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray> &vertexArray, const uint32_t indexCount)
    {
        LEV_PROFILE_FUNCTION();

        vertexArray->Bind();
        const uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
    }

    void OpenGLRendererAPI::DrawLines(const Ref <VertexArray> &vertexArray, uint32_t vertexCount)
    {
        LEV_PROFILE_FUNCTION();

        vertexArray->Bind();
        glDrawArrays(GL_LINES, 0, vertexCount);
    }

    void OpenGLRendererAPI::SetLineWidth(float width)
    {
        glLineWidth(width);
    }

    void OpenGLRendererAPI::SetViewport(const int x, const int y, const uint32_t width, const uint32_t height)
    {
        LEV_PROFILE_FUNCTION();

        glViewport(x, y, width, height);
    }

    void OpenGLRendererAPI::SetDepthFunc(DepthFunc depthFunc)
    {
        LEV_PROFILE_FUNCTION();

        auto glDepthFuncValue = ConvertDepthFuncToGl(depthFunc);

        glDepthFunc(glDepthFuncValue);
    }
}