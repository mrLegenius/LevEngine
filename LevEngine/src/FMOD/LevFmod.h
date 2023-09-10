#pragma once
#include "fmod_common.h"
#include "fmod_studio.hpp"
#include "fmod_errors.h"
#include "../Kernel/Logger.h"


namespace LevEngine
{
    class Entity;
}

namespace DirectX
{
    namespace SimpleMath
    {
        struct Vector3;
    }
}

namespace LevEngine
{
    struct Transform;
}

namespace LevEngine::LevFmod
{
    class LevFmod
    {
    public:
        struct EventInfo {
            // Entity to which this event is attached
            Entity *entity = nullptr;
        };
        
        LevFmod();
        ~LevFmod();
    
        void Init(int numOfChannels, int studioFlags, int flags) const;
        void Update();
        void ClearChannelRefs();
        void SetListenerAttributes();

        void PlayOneShot(const std::string &eventName, const Entity *entity);

        FMOD_3D_ATTRIBUTES Get3DAttributes(FMOD_VECTOR pos, FMOD_VECTOR up, FMOD_VECTOR forward, FMOD_VECTOR vel);
        FMOD_VECTOR ToFmodVector(DirectX::SimpleMath::Vector3 vec);
        void UpdateInstance3DAttributes(FMOD::Studio::EventInstance *i, const Entity *entity);
        EventInfo* GetEventInfo(FMOD::Studio::EventInstance* eventInstance);
        void ReleaseOneEvent(FMOD::Studio::EventInstance *eventInstance);

        /* Bank functions */
        std::string LoadBank(const std::string &pathToBank, int flags);
        void UnloadBank(const std::string &pathToBank);

    private:
        FMOD::Studio::EventInstance *CreateInstance(std::string eventPath, bool isOneShot, Entity *entity);
        FMOD::Studio::EventInstance *CreateInstance(const FMOD::Studio::EventDescription *eventDesc, bool isOneShot,
                                                    Entity *entity);

        inline int CheckErrors(FMOD_RESULT result) const
        {
            if (result != FMOD_OK) {
                const std::string err = std::string("FMOD Sound System: ") + std::string(FMOD_ErrorString(result));
                Log::Error(err);
                return 0;
            }
            return 1;
        }
        
        FMOD::Studio::System *m_system;
        FMOD::System *m_coreSystem;
        LevFmod* m_singleton;
        std::unordered_map<std::string, FMOD::Studio::EventDescription *> m_eventDescriptions;
        std::unordered_map<uint64_t, FMOD::Studio::EventInstance *> m_events;
        std::unordered_map<std::string, FMOD::Studio::Bank *> m_banks;
    };
}

