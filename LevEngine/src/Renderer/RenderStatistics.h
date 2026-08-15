#pragma once

#include "Kernel/Core.h"
#include "DataTypes/String.h"
#include "DataTypes/Vector.h"

namespace LevEngine
{
    struct RenderPassStatistics
    {
        String Name;
        uint32_t DrawCalls = 0;
        uint32_t Triangles = 0;
        uint32_t VertexBuffersCreated = 0;
        uint32_t IndexBuffersCreated = 0;
        uint32_t ConstantBufferUpdates = 0;
    };

    // Counts what a frame actually submitted, per pass, on the CPU side: draws, triangles, and the
    // GPU buffers created while the pass was running.
    //
    // The last one is the reason this exists. Mesh upload happens on the main thread in the middle of
    // a pass, so it lands inside that pass' GPU timestamp and makes a cheap pass look expensive. A
    // timer cannot tell those apart, a counter can.
    //
    // Everything is written under one mutex because draws are recorded from job threads through the
    // deferred contexts, and a few thousand uncontended locks a frame do not show up in a profile.
    class LEV_API RenderStatistics
    {
    public:
        // Starts a new frame the first time it is called for a given frame number, so the several
        // Render calls an editor frame makes accumulate into one set of numbers instead of
        // overwriting each other.
        static void BeginFrame(uint64_t frameNumber);

        static void BeginPass(const String& name);
        static void EndPass();

        static void CountDrawCall(uint32_t triangles);
        static void CountVertexBufferCreated();
        static void CountIndexBufferCreated();
        static void CountConstantBufferUpdate();

        //<--- The last completed frame. Empty until one has been rendered ---<<
        [[nodiscard]] static Vector<RenderPassStatistics> GetLastFrame();
        [[nodiscard]] static uint64_t GetLastFrameNumber();

        //<--- Counting is off until something asks for the numbers, see AgentBridge ---<<
        static void SetEnabled(bool enabled);
        [[nodiscard]] static bool IsEnabled();
    };

    // Opens a pass for the lifetime of the scope. RenderPass::Execute uses it, so a pass that throws
    // or returns early does not leave the counters attributed to it forever.
    class LEV_API ScopedRenderPassStatistics
    {
    public:
        explicit ScopedRenderPassStatistics(const String& name) { RenderStatistics::BeginPass(name); }
        ~ScopedRenderPassStatistics() { RenderStatistics::EndPass(); }

        ScopedRenderPassStatistics(const ScopedRenderPassStatistics&) = delete;
        ScopedRenderPassStatistics& operator=(const ScopedRenderPassStatistics&) = delete;
    };
}
