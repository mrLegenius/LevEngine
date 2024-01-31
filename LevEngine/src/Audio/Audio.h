#pragma once
#include "fmod_common.h"
#include "fmod_studio.hpp"
#include "fmod_errors.h"
#include "Kernel/Logger.h"
#include "Scene/Entity.h"
#include <source_location>


namespace LevEngine
{
    typedef intptr_t EventInstanceHandle;
    struct AudioListenerComponent;
    struct Transform;

    class Audio final
    {
    public:
        static const int MaxAudioChannelCount = 1024;

        struct EventInfo {
            // Entity to which this event is attached
            Entity entity = Entity();
        };
        
        static bool Init(int numOfChannels, int studioFlags, int flags);
        static void Shutdown();


        static void Update();


        /* Audio listeners */
        static void AddListener(AudioListenerComponent* listenerComponent);
        static void RemoveListener(AudioListenerComponent* listenerComponent);
        static void RemoveAllListeners();
        static void SetListenerAttributes();


        /* One-shot sound */
        static void PlayOneShot(const String &eventName, Entity entity);


        /* Non-one-shot sound */
        [[nodiscard]] static EventInstanceHandle CreateSound(const String& eventName, Entity entity, bool playOnCreate);
        static void PlaySound(EventInstanceHandle handle);
        static void StopSound(EventInstanceHandle handle, bool forceImmediateStop = false);
        static bool IsPaused(EventInstanceHandle handle);
        static void SetPause(EventInstanceHandle handle, bool isPaused);
        static bool IsPlaying(EventInstanceHandle handle);
        static bool IsStartingPlaying(EventInstanceHandle handle);
        

        /* Bank functions */
        static void LoadBank(const String &pathToBank, bool preloadSampleData = false);
        static void UnloadBank(const String &pathToBank);
        static void UnloadAllBanks();
        static bool IsBankLoaded(const String& pathToBank);
        static bool IsBankLoading(const String& pathToBank);


        /* Parameters */
        static float GetGlobalParameterByName(const String& parameterName);
        static void SetGlobalParameterByName(const String& parameterName, float value);
        static float GetGlobalParameterByID(unsigned int idHalf1, unsigned int idHalf2);
        static void SetGlobalParameterByID(unsigned int idHalf1, unsigned int idHalf2, float value);
        static float GetEventParameterByName(EventInstanceHandle handle, const String& parameterName);
        static void SetEventParameterByName(EventInstanceHandle handle, const String& parameterName, float value);
        static float GetEventParameterByID(EventInstanceHandle handle, unsigned int idHalf1, unsigned int idHalf2);
        static void SetEventParameterByID(EventInstanceHandle handle, unsigned int idHalf1, unsigned int idHalf2, float value);


        /* Event lifetime */
        static void ReleaseOneEvent(EventInstanceHandle handle, bool forceImmediateStop = false);
        static void ReleaseAllEvents();
        static void ReleaseAll();


        /* Helper functions */
        // Returns eventName with "event:/" prefix
        static String GetPrefixedEventName(const String& eventName);

    private:
        /* Event playback */
        static void StopEventInstance(FMOD::Studio::EventInstance* eventInstance, bool forceImmediateStop = false);

        /* Event lifetime */
        static FMOD::Studio::EventInstance *CreateInstance(String eventPath, bool isOneShot, Entity entity);
        static FMOD::Studio::EventInstance *CreateInstance(const FMOD::Studio::EventDescription *eventDesc, bool isOneShot,
                                                    Entity entity);


        /* Event info */
        static EventInfo* GetEventInfo(FMOD::Studio::EventInstance* eventInstance);
        static FMOD_STUDIO_PLAYBACK_STATE GetPlaybackState(FMOD::Studio::EventInstance* eventInstance);


        /* Event 3D attributes */
        static FMOD_3D_ATTRIBUTES Get3DAttributes(FMOD_VECTOR pos, FMOD_VECTOR up, FMOD_VECTOR forward, FMOD_VECTOR vel);
        static FMOD_VECTOR ToFmodVector(Vector3 vec);
        static FMOD_3D_ATTRIBUTES Get3DAttributes(const Transform& transform);
        static void UpdateInstance3DAttributes(FMOD::Studio::EventInstance* i, const Entity entity);


        static bool IsBankRegistered(const String& pathToBank);

        static int CheckErrors(FMOD_RESULT result, const std::source_location& location = std::source_location::current())
        {
#ifdef _DEBUG
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
#else
            return 0;
#endif // DEBUG
        }

        inline static const String EventNamePrefix = "event:/";
        
        inline static FMOD::Studio::System *m_System;

        inline static UnorderedMap<String, FMOD::Studio::EventDescription *> m_EventDescriptions;
        inline static UnorderedMap<EventInstanceHandle, FMOD::Studio::EventInstance*> m_Events;
        inline static UnorderedMap<String, FMOD::Studio::Bank*> m_Banks;
        inline static Vector<AudioListenerComponent*> m_Listeners;

        inline static Vector<EventInstanceHandle> m_EventsToRelease;

        inline static float m_DistanceScale = 1.0f;
        inline static bool m_IsInitialized = false;
    };
}

