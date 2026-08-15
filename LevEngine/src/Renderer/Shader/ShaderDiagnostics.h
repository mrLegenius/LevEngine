#pragma once

#include "Kernel/Core.h"
#include "DataTypes/String.h"
#include "DataTypes/Vector.h"

namespace LevEngine
{
    // Compilation errors, kept instead of only being logged.
    //
    // A hot reload that fails writes to the log and carries on with the old shader, which looks
    // exactly like a reload that changed nothing. Recording the errors lets a caller reload a file
    // and be told whether it took.
    class LEV_API ShaderDiagnostics
    {
    public:
        struct Entry
        {
            String Path;
            //<--- Vertex, Pixel, Geometry or Compute, whichever stage failed ---<<
            String Stage;
            String Message;
        };

        static void Clear();
        static void Record(const String& path, const String& stage, const String& message);
        [[nodiscard]] static Vector<Entry> GetEntries();
    };
}
