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

        String ReadString();

        template<typename T>
        Vector<T> ReadVector();

        template <typename T, size_t N>
        Array<T, N> ReadArray();

        template<typename T>
        void ReadForEach(Action<T> onRead);
        [[nodiscard]] bool IsOpen() const { return stream.is_open(); }

    private:

        void ReadBlock(char* data, uint32_t size);
        
        std::ifstream stream{}; 
    };
}

#include "BinaryReader.inl"