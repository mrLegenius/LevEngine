#pragma once
#include <filesystem>

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

namespace LevEngine
{
inline void CopyRecursively(const Path& source, const Path& destination) noexcept
{
    try
    {
        copy(source, destination, std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
    }
    catch (std::exception& e)
    {
        Log::CoreWarning("Failed to copy from {0} to {1}. Error: {2}", source.string().c_str(), destination.string().c_str(), e.what());
    }
}
}