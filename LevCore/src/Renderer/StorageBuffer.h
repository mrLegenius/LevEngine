#pragma once

namespace LevEngine
{
    class StorageBuffer
    {
    public:
        virtual ~StorageBuffer() {}
        virtual void SetData(const void* data, uint32_t size) = 0;

        static Ref<StorageBuffer> Create(uint32_t binding);
    };
}


