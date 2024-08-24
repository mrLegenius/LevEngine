#pragma once

namespace LevEngine
{
    template <typename T>
    T BinaryReader::Read()
    {
        char data[sizeof(T)];
        stream.read(data, sizeof(T));
        return *reinterpret_cast<T*>(data);
    }

    template <typename T>
    Vector<T> BinaryReader::ReadVector()
    {
        auto count = Read<size_t>();

        Vector<T> vector;
        if (count > 0x80000000)
        {
            Log::CoreError("Trying to read too large sequence with size {}", count);
            return vector;
        }
        
        vector.reserve(count);

        for (int i = 0; i < count; ++i)
        {
            auto element = Read<T>();
            vector.push_back(element);
        }

        return vector;
    }

    template <typename T, size_t N>
    Array<T, N> BinaryReader::ReadArray()
    {
        Array<T, N> array;

        for (int i = 0; i < N; ++i)
            array[i] = Read<T>();

        return array;
    }

    template <typename T>
    void BinaryReader::ReadForEach(Action<T> onRead)
    {
        auto count = Read<size_t>();

        for (int i = 0; i < count; ++i)
        {
            auto element = Read<T>();
            onRead(element);
        }
    }
}
