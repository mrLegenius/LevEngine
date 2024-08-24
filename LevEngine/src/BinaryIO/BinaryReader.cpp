#include "levpch.h"
#include "BinaryReader.h"

namespace LevEngine
{
    BinaryReader::BinaryReader(const Path& path) : stream(path, std::ios::in | std::ios::binary)
    {
    }

    BinaryReader::~BinaryReader()
    {
        if (stream.is_open())
            stream.close();
    }

    void BinaryReader::ReadBlock(char* data, uint32_t size)
    {
        stream.read(data, size);
    }

    String BinaryReader::ReadString()
    {
        auto size = Read<size_t>();
        String result;
        result.resize(size);
        stream.read(result.begin(), size);
        return result;
    }
}
