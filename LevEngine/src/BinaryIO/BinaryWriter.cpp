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
    
    void BinaryWriter::WriteString(String data)
    {
        size_t size = data.size();
        Write<size_t>(size);
        stream.write(data.begin(), size);
    }
}
