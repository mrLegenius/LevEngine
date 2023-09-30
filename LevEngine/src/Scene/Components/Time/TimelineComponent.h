#pragma once
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
    class Timeline;
    
    REGISTER_PARSE_TYPE(TimelineComponent);

    struct TimelineComponent
    {
        TimelineComponent();
        
        bool isLooping{};
        double duration{};
        double timeScale{};
        bool playOnInit{true};
        Ref<Timeline> timeline = nullptr;

        void Init();
        
        [[nodiscard]] bool GetIsLooping() const;
        void SetIsLooping(bool isLooping);
        [[nodiscard]] double GetDuration() const;
        void SetDuration(double duration);
        [[nodiscard]] double GetTimeScale() const;
        void SetTimeScale(double timeScale);
        [[nodiscard]] bool GetPlayOnInit() const;
        void SetPlayOnInit(bool playOnInit);
        [[nodiscard]] double GetElapsedTime() const;
        bool IsInitialized() const;
        void ResetInit();
        bool IsPlaying() const;
        void Play() const;
        void Pause() const;
        void Stop() const;

    private:
        bool m_IsInited{};
    };
}
