#include "levpch.h"
#include "BinaryWriter.h"

namespace LevEngine
{
    BinaryWriter::BinaryWriter(const Path& path) : stream(path, std::ios::out | std::ios::binary | std::ios::trunc)
    {
    }

    BinaryWriter::~BinaryWriter()
    {
        if (stream.is_open())
            stream.close();
    }
}
