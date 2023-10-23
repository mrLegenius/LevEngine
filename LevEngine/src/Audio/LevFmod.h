#pragma once
#include "fmod_common.h"
#include "fmod_studio.hpp"
#include "fmod_errors.h"
#include "Kernel/Logger.h"
#include "DataTypes/UnorderedMap.h"
#include "DataTypes/Vector.h"
#include "Scene/Entity.h"
#include <source_location>

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
    class LevFmod final
    {
    public:
        struct EventInfo {
            // Entity to which this event is attached
            Entity entity = Entity();
        };
        
        LevFmod();
        ~LevFmod();
    
        bool Init(int numOfChannels, int studioFlags, int flags) const;
        void Update();


        /* Audio listeners */
        void AddListener(AudioListenerComponent* listenerComponent);
        void RemoveListener(AudioListenerComponent* listenerComponent);
        void SetListenerAttributes();


        /* One-shot sound */
        void PlayOneShot(const String &eventName, Entity entity);


        /* Non-one-shot sound */
        [[nodiscard]] intptr_t CreateSound(const String& eventName, Entity entity, bool playOnCreate);
        void PlaySound(intptr_t instanceHandle);
        void StopSound(intptr_t instanceHandle, bool forceImmediateStop = false);
        void SetPause(intptr_t instanceHandle, bool isPaused);
        bool IsPlaying(intptr_t instanceHandle);
        bool IsStartingPlaying(intptr_t instanceHandle);
        

        /* Bank functions */
        void LoadBank(const String &pathToBank, bool preloadSampleData = false);
        void UnloadBank(const String &pathToBank);
        void UnloadAllBanks();
        bool IsBankLoaded(const String& pathToBank);
        bool IsBankLoading(const String& pathToBank);


        /* Parameters */
        float GetGlobalParameterByName(const String& parameterName);
        void SetGlobalParameterByName(const String& parameterName, float value);
        float GetGlobalParameterByID(unsigned int idHalf1, unsigned int idHalf2);
        void SetGlobalParameterByID(unsigned int idHalf1, unsigned int idHalf2, float value);
        float GetEventParameterByName(intptr_t instanceHandle, const String& parameterName);
        void SetEventParameterByName(intptr_t instanceHandle, const String& parameterName, float value);
        float GetEventParameterByID(intptr_t instanceHandle, unsigned int idHalf1, unsigned int idHalf2);
        void SetEventParameterByID(intptr_t instanceHandle, unsigned int idHalf1, unsigned int idHalf2, float value);


        /* Event lifetime */
        void ReleaseOneEvent(intptr_t instanceHandle, bool forceImmediateStop = false);
        void ReleaseAllEvents();
        void ReleaseAll();


        /* Helper functions */
        // Returns eventName with "event:/" prefix
        String GetPrefixedEventName(const String& eventName);

    private:
        /* Event playback */
        void StopEventInstance(FMOD::Studio::EventInstance* eventInstance, bool forceImmediateStop = false);

        /* Event lifetime */
        FMOD::Studio::EventInstance *CreateInstance(String eventPath, bool isOneShot, Entity entity);
        FMOD::Studio::EventInstance *CreateInstance(const FMOD::Studio::EventDescription *eventDesc, bool isOneShot,
                                                    Entity entity);


        /* Event info */
        EventInfo* GetEventInfo(FMOD::Studio::EventInstance* eventInstance);
        FMOD_STUDIO_PLAYBACK_STATE GetPlaybackState(FMOD::Studio::EventInstance* eventInstance);


        /* Event 3D attributes */
        FMOD_3D_ATTRIBUTES Get3DAttributes(FMOD_VECTOR pos, FMOD_VECTOR up, FMOD_VECTOR forward, FMOD_VECTOR vel);
        FMOD_VECTOR ToFmodVector(DirectX::SimpleMath::Vector3 vec);
        FMOD_3D_ATTRIBUTES Get3DAttributes(const Transform& transform);
        void UpdateInstance3DAttributes(FMOD::Studio::EventInstance* i, const Entity entity);


        bool IsBankRegistered(const String pathToBank);

        int CheckErrors(FMOD_RESULT result, const std::source_location& location = std::source_location::current()) const
        {
            if (result != FMOD_OK) 
            {
                Log::CoreError("{}({}:{}) {}: FMOD Sound System: {}", 
                    location.file_name(), 
                    location.line(), 
                    location.column(), 
                    location.function_name(), 
                    FMOD_ErrorString(result));

                return 0;
            }
            return 1;
        }
        
        FMOD::Studio::System *m_System;

        UnorderedMap<String, FMOD::Studio::EventDescription *> m_EventDescriptions;
        UnorderedMap<intptr_t, FMOD::Studio::EventInstance*> m_Events;
        UnorderedMap<String, FMOD::Studio::Bank*> m_Banks;
        Vector<AudioListenerComponent*> m_Listeners;

        Vector<intptr_t> m_EventsToRelease;

        float m_DistanceScale = 1.0f;
        bool m_ListenerWarning = true;
    };
}

