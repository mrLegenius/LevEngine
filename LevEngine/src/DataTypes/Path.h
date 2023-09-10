#pragma once

using Path = std::filesystem::path;

template <>
struct eastl::hash<std::filesystem::path>
{
    std::size_t operator()(const std::filesystem::path& k) const noexcept
    {
        using std::size_t;
        using eastl::hash;
        using eastl::string;
        return std::hash<std::filesystem::path>()(k);
    }
};
