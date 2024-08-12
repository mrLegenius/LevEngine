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
}
