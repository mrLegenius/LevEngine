#pragma once

#include "Renderer/StorageBuffer.h"

namespace LevEngine
{
    class OpenGLStorageBuffer : public StorageBuffer
    {
    public:
        OpenGLStorageBuffer(uint32_t binding);
        ~OpenGLStorageBuffer() override;
        void SetData(const void* data, uint32_t size) override;
    private:
        uint32_t m_RendererID = 0;
    };
}


