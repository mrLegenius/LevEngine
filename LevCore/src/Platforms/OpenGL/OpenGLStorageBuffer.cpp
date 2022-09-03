#include "OpenGLStorageBuffer.h"

#include <glad/gl.h>

namespace LevEngine
{
    OpenGLStorageBuffer::OpenGLStorageBuffer(uint32_t binding)
    {
        glCreateBuffers(1, &m_RendererID);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, m_RendererID);
    }

    OpenGLStorageBuffer::~OpenGLStorageBuffer()
    {
        glDeleteBuffers(1, &m_RendererID);
    }

    void OpenGLStorageBuffer::SetData(const void* data, uint32_t size)
    {
        glNamedBufferData(m_RendererID, size, data, GL_DYNAMIC_DRAW);
    }
}