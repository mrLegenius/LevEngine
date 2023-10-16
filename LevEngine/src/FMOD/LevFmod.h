#pragma once
#include "fmod_common.h"
#include "fmod_studio.hpp"
#include "fmod_errors.h"
#include "../Kernel/Logger.h"
#include "DataTypes/UnorderedMap.h"
#include "DataTypes/Vector.h"
#include "Scene/Entity.h"

namespace LevEngine
{
    struct AudioListenerComponent;
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

namespace LevEngine
{
    class LevFmod
    {
    public:
        struct Listener {
            // Entity to which this listener is attached
            Entity entity;

            // When true, locks the listener in place, disabling internal 3D attribute updates.
            // 3D attributes can still be manually set with a set3DAttributes call.
            bool listenerLock = false;
        };

        struct EventInfo {
            // Entity to which this event is attached
            Entity entity = Entity();
        };
        
        LevFmod();
        ~LevFmod();
    
        void Init(int numOfChannels, int studioFlags, int flags) const;
        void Update();


        /* Audio listeners */
        void AddListener(AudioListenerComponent* listenerComponent);
        void RemoveListener(AudioListenerComponent* listenerComponent);
        void SetListenerAttributes();


        /* One-shot sound */
        void PlayOneShot(const String &eventName, Entity entity);


        /* Non-one-shot sound */
        [[nodiscard]] FMOD::Studio::EventInstance* CreateSound(const String& eventName, Entity entity, bool playOnCreate);
        void PlaySound(FMOD::Studio::EventInstance* instance);
        void StopSound(FMOD::Studio::EventInstance* instance, 
            FMOD_STUDIO_STOP_MODE mode = FMOD_STUDIO_STOP_MODE::FMOD_STUDIO_STOP_ALLOWFADEOUT);
        void SetPause(FMOD::Studio::EventInstance* instance, bool isPaused);
        bool IsPlaying(FMOD::Studio::EventInstance* instance);

        EventInfo* GetEventInfo(FMOD::Studio::EventInstance* eventInstance);
        

        /* Bank functions */
        void LoadBank(const String &pathToBank, int flags);
        void UnloadBank(const String &pathToBank);
        void UnloadAllBanks();
        FMOD_STUDIO_LOADING_STATE GetBankLoadingStatus(const String& pathToBank);
        

        /* Event 3D attributes */
        FMOD_3D_ATTRIBUTES Get3DAttributes(FMOD_VECTOR pos, FMOD_VECTOR up, FMOD_VECTOR forward, FMOD_VECTOR vel);
        FMOD_VECTOR ToFmodVector(DirectX::SimpleMath::Vector3 vec);
        FMOD_3D_ATTRIBUTES Get3DAttributes(const Transform& transform);
        void UpdateInstance3DAttributes(FMOD::Studio::EventInstance* i, const Entity entity);


        /* Event parameters */
        float GetEventParameterByName(FMOD::Studio::EventInstance* eventInstance, const String& parameterName);
        void SetEventParameterByName(FMOD::Studio::EventInstance* eventInstance, const String& parameterName, float value);
        float GetEventParameterByID(FMOD::Studio::EventInstance* eventInstance, unsigned int idHalf1, unsigned int idHalf2);
        void SetEventParameterByID(FMOD::Studio::EventInstance* eventInstance, unsigned int idHalf1, unsigned int idHalf2, float value);


        /* Event lifetime */
        void ReleaseOneEvent(FMOD::Studio::EventInstance* eventInstance);
        void ReleaseAllEvents();
        void ReleaseAll();

    private:
        FMOD::Studio::EventInstance *CreateInstance(String eventPath, bool isOneShot, Entity entity);
        FMOD::Studio::EventInstance *CreateInstance(const FMOD::Studio::EventDescription *eventDesc, bool isOneShot,
                                                    Entity entity);
        bool IsBankRegistered(const String pathToBank);

        inline int CheckErrors(FMOD_RESULT result) const
        {
            if (result != FMOD_OK) {
                const String err = String("FMOD Sound System: ") + String(FMOD_ErrorString(result));
                Log::Error(err);
                return 0;
            }
            return 1;
        }
        
        FMOD::Studio::System *m_System;
        FMOD::System *m_CoreSystem;
        LevFmod* m_Singleton;

        UnorderedMap<String, FMOD::Studio::EventDescription *> m_EventDescriptions;
        UnorderedMap<uint64_t, FMOD::Studio::EventInstance*> m_Events;
        UnorderedMap<String, FMOD::Studio::Bank*> m_Banks;
        Vector<AudioListenerComponent*> m_Listeners;

        float m_DistanceScale = 1.0f;
        bool m_ListenerWarning = true;
    };
}

