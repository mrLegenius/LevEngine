#pragma once
#include "fmod_common.h"
#include "fmod_studio.hpp"
#include "fmod_errors.h"
#include "../Kernel/Logger.h"
#include "DataTypes/UnorderedMap.h"


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

        void PlayOneShot(const String &eventName, const Entity *entity);

        FMOD_3D_ATTRIBUTES Get3DAttributes(FMOD_VECTOR pos, FMOD_VECTOR up, FMOD_VECTOR forward, FMOD_VECTOR vel);
        FMOD_VECTOR ToFmodVector(DirectX::SimpleMath::Vector3 vec);
        void UpdateInstance3DAttributes(FMOD::Studio::EventInstance *i, const Entity *entity);
        EventInfo* GetEventInfo(FMOD::Studio::EventInstance* eventInstance);
        void ReleaseOneEvent(FMOD::Studio::EventInstance *eventInstance);

        /* Bank functions */
        String LoadBank(const String &pathToBank, int flags);
        void UnloadBank(const String &pathToBank);

    private:
        FMOD::Studio::EventInstance *CreateInstance(String eventPath, bool isOneShot, Entity *entity);
        FMOD::Studio::EventInstance *CreateInstance(const FMOD::Studio::EventDescription *eventDesc, bool isOneShot,
                                                    Entity *entity);

        inline int CheckErrors(FMOD_RESULT result) const
        {
            if (result != FMOD_OK) {
                const String err = String("FMOD Sound System: ") + String(FMOD_ErrorString(result));
                Log::Error(err);
                return 0;
            }
            return 1;
        }
        
        FMOD::Studio::System *m_system;
        FMOD::System *m_coreSystem;
        LevFmod* m_singleton;
        UnorderedMap<String, FMOD::Studio::EventDescription *> m_eventDescriptions;
        UnorderedMap<uint64_t, FMOD::Studio::EventInstance *> m_events;
        UnorderedMap<String, FMOD::Studio::Bank *> m_banks;
    };
}

