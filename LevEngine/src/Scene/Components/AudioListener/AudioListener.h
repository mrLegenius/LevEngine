#pragma once

namespace LevEngine
{
    struct AudioListenerComponent
    {
        static constexpr int MaxStringLength = 128;
        String eventName;

        AudioListenerComponent();
        AudioListenerComponent(const AudioListenerComponent&) = default;
    };
}
