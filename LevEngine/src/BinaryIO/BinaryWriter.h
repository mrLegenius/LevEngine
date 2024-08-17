#pragma once

namespace LevEngine
{
    class BinaryWriter
    {
    public:
        BinaryWriter(const Path& path);
        ~BinaryWriter();

        template <typename T>
        void Write(T data);

        void WriteString(String data);

        template <typename T>
        void WriteVector(Vector<T> vector);

        template <typename T, size_t N>
        void WriteArray(Array<T, N> array);

        template <typename T>
        void WriteForEach(size_t count, Func<size_t, T> dataGetter);

    private:
        std::ofstream stream{};
    };
}

#include "BinaryWriter.inl"
