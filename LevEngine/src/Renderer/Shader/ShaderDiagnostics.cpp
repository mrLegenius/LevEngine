#include "levpch.h"
#include "ShaderDiagnostics.h"

namespace LevEngine
{
    namespace
    {
        std::mutex s_Mutex;
        Vector<ShaderDiagnostics::Entry> s_Entries;

        //<--- A shader that fails to compile fails on every frame that tries to reload it, and
        //nothing is served by keeping every copy ---<<
        constexpr size_t k_MaxEntries = 256;
    }

    void ShaderDiagnostics::Clear()
    {
        std::lock_guard lock{ s_Mutex };
        s_Entries.clear();
    }

    void ShaderDiagnostics::Record(const String& path, const String& stage, const String& message)
    {
        std::lock_guard lock{ s_Mutex };

        if (s_Entries.size() >= k_MaxEntries)
            s_Entries.erase(s_Entries.begin());

        s_Entries.push_back(Entry{ .Path = path, .Stage = stage, .Message = message });
    }

    Vector<ShaderDiagnostics::Entry> ShaderDiagnostics::GetEntries()
    {
        std::lock_guard lock{ s_Mutex };
        return s_Entries;
    }
}
