#pragma once

namespace LevEngine
{
    class BinaryReader
    {
    public:
        BinaryReader(const Path& path);
        ~BinaryReader();

        template<typename T>
        T Read();

        template<typename T>
        Vector<T> ReadVector();

        template <typename T, size_t N>
        Array<T, N> ReadArray();

        template<typename T>
        void ReadForEach(Action<T> onRead);

    private:
        std::ifstream stream{}; 
    };
}

#include "BinaryReader.inl"