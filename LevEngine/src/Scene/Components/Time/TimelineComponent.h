#pragma once
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
    class Timeline;
    
    REGISTER_PARSE_TYPE(TimelineComponent);

    struct TimelineComponent
    {
        TimelineComponent();
        ~TimelineComponent();
        
        bool playOnInit{true};
        Ref<Timeline> timeline = nullptr;

        void Init();
        bool IsInitialized() const;
        void ResetInit();

        bool IsPlaying() const;
        void Play() const;
        void Pause() const;
        void Stop() const;
        [[nodiscard]] bool GetIsLooping() const;
        void SetIsLooping(bool isLooping) const;
        [[nodiscard]] double GetDuration() const;
        void SetDuration(double duration);
        [[nodiscard]] double GetTimeScale() const;
        void SetTimeScale(double timeScale);
        [[nodiscard]] bool GetPlayOnInit() const;
        void SetPlayOnInit(bool playOnInit);
        [[nodiscard]] double GetElapsedTime() const;

    private:
        bool m_IsInited{};
    };
}
