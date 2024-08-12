#pragma once

namespace LevEngine
{
    template <typename T>
    void BinaryWriter::Write(T data)
    {
        stream.write(reinterpret_cast<char*>(&data), sizeof(T));
    }

    template <typename T>
    void BinaryWriter::WriteVector(Vector<T> vector)
    {
        Write(vector.size());

        for (auto element : vector)
        {
            Write<T>(element);
        }
    }

    template <typename T, size_t N>
    void BinaryWriter::WriteArray(Array<T, N> array)
    {
        for (int i = 0; i < N; ++i)
            Write<T>(array[i]);
    }

    template <typename T>
    void BinaryWriter::WriteForEach(size_t count, Func<size_t, T> dataGetter)
    {
        Write(count);

        for (size_t i = 0; i < count; ++i)
        {
            auto element = dataGetter(i);
            Write(element);
        }
    }
}
