#pragma once

namespace LevEngine
{
	// Defines either a semantic (HLSL) or an input index (GLSL/HLSL)
	// to bind an input buffer.
    struct BufferBinding
    {
        BufferBinding() : Index(0) { }

        BufferBinding(const String& name, const unsigned int index) : Name(name) , Index(index) { }

        // Provide the < operator for STL containers.
        bool operator<(const BufferBinding& rhs) const
        {
            if (Name < rhs.Name) return true;
            if (Name > rhs.Name) return false;
            // Names are equal...
            if (Index < rhs.Index) return true;
            if (Index > rhs.Index) return false;
            // Indexes are equal...

            return false;
        }

        String Name;
        unsigned int Index;
    };
}
