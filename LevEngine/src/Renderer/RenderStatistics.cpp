#include "levpch.h"
#include "RenderStatistics.h"

namespace LevEngine
{
    namespace
    {
        std::mutex s_Mutex;

        Vector<RenderPassStatistics> s_Current;
        Vector<RenderPassStatistics> s_Last;

        //<--- Pass names in the order they were opened. Passes nest, so this is a stack ---<<
        Vector<int> s_OpenPasses;

        uint64_t s_FrameNumber = 0;
        uint64_t s_LastFrameNumber = 0;

        //<--- Index of the bucket everything counted outside a pass goes into, -1 until needed ---<<
        int s_OutsideIndex = -1;

        bool s_Enabled = false;

        constexpr const char* k_OutsideAnyPass = "<outside passes>";

        //<--- The mutex is already held by every caller of this ---<<
        RenderPassStatistics& CurrentBucket()
        {
            if (!s_OpenPasses.empty())
                return s_Current[s_OpenPasses.back()];

            if (s_OutsideIndex < 0)
            {
                s_Current.push_back(RenderPassStatistics{ .Name = k_OutsideAnyPass });
                s_OutsideIndex = static_cast<int>(s_Current.size()) - 1;
            }

            return s_Current[s_OutsideIndex];
        }
    }

    void RenderStatistics::BeginFrame(const uint64_t frameNumber)
    {
        if (!s_Enabled) return;

        std::lock_guard lock{ s_Mutex };

        if (frameNumber == s_FrameNumber) return;

        s_Last = s_Current;
        s_LastFrameNumber = s_FrameNumber;

        s_Current.clear();
        s_OpenPasses.clear();
        s_OutsideIndex = -1;
        s_FrameNumber = frameNumber;
    }

    void RenderStatistics::BeginPass(const String& name)
    {
        if (!s_Enabled) return;

        std::lock_guard lock{ s_Mutex };

        //<--- A pass drawn once per viewport is one entry with the totals of both ---<<
        for (int i = 0; i < static_cast<int>(s_Current.size()); ++i)
        {
            if (s_Current[i].Name != name) continue;

            s_OpenPasses.push_back(i);
            return;
        }

        s_Current.push_back(RenderPassStatistics{ .Name = name });
        s_OpenPasses.push_back(static_cast<int>(s_Current.size()) - 1);
    }

    void RenderStatistics::EndPass()
    {
        if (!s_Enabled) return;

        std::lock_guard lock{ s_Mutex };

        if (!s_OpenPasses.empty())
            s_OpenPasses.pop_back();
    }

    void RenderStatistics::CountDrawCall(const uint32_t triangles)
    {
        if (!s_Enabled) return;

        std::lock_guard lock{ s_Mutex };

        auto& bucket = CurrentBucket();
        bucket.DrawCalls++;
        bucket.Triangles += triangles;
    }

    void RenderStatistics::CountVertexBufferCreated()
    {
        if (!s_Enabled) return;

        std::lock_guard lock{ s_Mutex };
        CurrentBucket().VertexBuffersCreated++;
    }

    void RenderStatistics::CountIndexBufferCreated()
    {
        if (!s_Enabled) return;

        std::lock_guard lock{ s_Mutex };
        CurrentBucket().IndexBuffersCreated++;
    }

    void RenderStatistics::CountConstantBufferUpdate()
    {
        if (!s_Enabled) return;

        std::lock_guard lock{ s_Mutex };
        CurrentBucket().ConstantBufferUpdates++;
    }

    Vector<RenderPassStatistics> RenderStatistics::GetLastFrame()
    {
        std::lock_guard lock{ s_Mutex };
        return s_Last;
    }

    uint64_t RenderStatistics::GetLastFrameNumber()
    {
        std::lock_guard lock{ s_Mutex };
        return s_LastFrameNumber;
    }

    void RenderStatistics::SetEnabled(const bool enabled)
    {
        std::lock_guard lock{ s_Mutex };

        if (s_Enabled == enabled) return;

        s_Enabled = enabled;

        s_Current.clear();
        s_OpenPasses.clear();
        s_OutsideIndex = -1;

        if (!enabled)
            s_Last.clear();
    }

    bool RenderStatistics::IsEnabled() { return s_Enabled; }
}
