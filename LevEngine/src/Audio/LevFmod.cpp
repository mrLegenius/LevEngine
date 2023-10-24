#include "levpch.h"
#include "LevFmod.h"

#include "fmod_studio.hpp"
#include "Scene/Components/Audio/AudioListener.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    LevFmod::LevFmod()
    {
        m_System = nullptr;
        CheckErrors(FMOD::Studio::System::create(&m_System));
    }

    LevFmod::~LevFmod()
    {
        ReleaseAll();
        CheckErrors(m_System->release());
    }

    bool LevFmod::Init(int numOfChannels, int studioFlags, int flags) const
    {
        // initialize FMOD Studio and FMOD Core System with provided flags
        if (CheckErrors(m_System->initialize(numOfChannels, studioFlags, flags, nullptr))) 
        {
            Log::Info("FMOD Sound System: Successfully initialized");

            if (studioFlags & FMOD_STUDIO_INIT_LIVEUPDATE)
            {
                Log::Info("FMOD Sound System: Live update enabled!");
            }
            
            return true;
        }
        else
        {
            Log::Error("FMOD Sound System: Failed to initialize");
        }

        AudioListenerComponent unused;
        return false;
    }

    void LevFmod::Update()
    {
        for (auto iterator = m_Events.begin(); iterator != m_Events.end(); iterator++) {
            FMOD::Studio::EventInstance *eventInstance = iterator->second;
            const EventInfo *eventInfo = GetEventInfo(eventInstance);

            // cleanup events from destroyed sources
            if (!eventInfo || !eventInfo->entity || !eventInfo->entity.HasComponent<Transform>()) 
            {
                m_EventsToRelease.emplace_back(iterator->first);
                continue;
            }

            UpdateInstance3DAttributes(eventInstance, eventInfo->entity);
        }

        for (auto instanceHandle : m_EventsToRelease)
        {
            ReleaseOneEvent(instanceHandle);
        }

        m_EventsToRelease.clear();

        // update listener position
        SetListenerAttributes();

        // finally, dispatch an update call to FMOD
        CheckErrors(m_System->update());
    }

#pragma region Audio Listeners
    void LevFmod::AddListener(AudioListenerComponent* listenerComponent)
    {
        if (m_Listeners.size() == FMOD_MAX_LISTENERS)
        {
            Log::Warning("FMOD Sound System: Could not add new listener. System already at max listeners.");
            return;
        }

        m_Listeners.emplace_back(listenerComponent);
        CheckErrors(m_System->setNumListeners(m_Listeners.size()));
        Log::Info("FMOD Sound System: Added new listener. Current listener count: {}.", m_Listeners.size());
    }

    void LevFmod::RemoveListener(AudioListenerComponent* listenerComponent)
    {
        auto iterator = eastl::find(m_Listeners.begin(), m_Listeners.end(), listenerComponent);

        if (iterator == m_Listeners.end())
        {
            Log::Warning("FMOD Sound System: Tried to remove an unknown listener.");
        }

        int listenerIdx = iterator - m_Listeners.begin();
        m_Listeners.erase(iterator);

        int newListenersCount = (int) m_Listeners.size();
        
        // Minimum listener count in FMOD is 1. Setting count to 0 will return an error code.
        CheckErrors(m_System->setNumListeners(newListenersCount == 0 ? 1 : newListenersCount));
        Log::Info("FMOD Sound System: Removed a listener with index {}. Current listener count: {}.",
            listenerIdx, m_Listeners.size());
    }

    void LevFmod::SetListenerAttributes()
    {
        if (m_Listeners.empty()) 
        {
            if (m_ListenerWarning) 
            {
                Log::Error("FMOD Sound System: No listeners are set!");
                m_ListenerWarning = false;
            }
            return;
        }

        for (int listenerIdx = 0; listenerIdx < m_Listeners.size(); listenerIdx++) {
            const auto listener = m_Listeners[listenerIdx];
            auto entity = listener->attachedToEntity;

            if (!entity)
            {
                continue;
            }

            if (entity.HasComponent<Transform>())
            {
                Transform transform = entity.GetComponent<Transform>();
                const auto attr = Get3DAttributes(transform);

                CheckErrors(m_System->setListenerAttributes(listenerIdx, &attr));
            }
        }
    }
#pragma endregion

#pragma region One-Shot Sounds
    void LevFmod::PlayOneShot(const String& eventName, const Entity entity)
    {
        String prefixedEventName = GetPrefixedEventName(eventName);

        FMOD::Studio::EventInstance* instance = CreateInstance(prefixedEventName, true, entity);
        if (instance) 
        {
            if (entity)
            {
                UpdateInstance3DAttributes(instance, entity);
            }

            CheckErrors(instance->start());
            CheckErrors(instance->release());
        }
    }
#pragma endregion

#pragma region Non-One-Shot Sounds
    intptr_t LevFmod::CreateSound(const String& eventName, Entity entity, bool playOnCreate)
    {
        String prefixedEventName = GetPrefixedEventName(eventName);

        FMOD::Studio::EventInstance* instance = CreateInstance(prefixedEventName, false, entity);
        if (instance)
        {
            intptr_t instanceHandle = reinterpret_cast<intptr_t>(instance);

            if (entity)
            {
                UpdateInstance3DAttributes(instance, entity);
            }

            if (playOnCreate)
            {
                PlaySound(instanceHandle);
            }

            return instanceHandle;
        }

        return 0;
    }

    void LevFmod::PlaySound(intptr_t instanceHandle)
    {
        auto iterator = m_Events.find(instanceHandle);

        if (iterator != m_Events.end())
        {
            CheckErrors(iterator->second->start());
        }
        else
        {
            Log::Warning("Attempted to play a sound using an invalid handle");
        }
    }

    void LevFmod::StopSound(intptr_t instanceHandle, bool forceImmediateStop)
    {
        auto iterator = m_Events.find(instanceHandle);

        if (iterator != m_Events.end())
        {
            StopEventInstance(iterator->second, forceImmediateStop);
        }
        else
        {
            Log::Warning("Attempted to stop a sound using an invalid handle");
        }
    }

    void LevFmod::SetPause(intptr_t instanceHandle, bool isPaused)
    {
        auto iterator = m_Events.find(instanceHandle);

        if (iterator != m_Events.end())
        {
            CheckErrors(iterator->second->setPaused(isPaused));
        }
        else
        {
            Log::Warning("Attempted to {} a sound using an invalid handle", isPaused ? "pause" : "unpause");
        }
    }

    bool LevFmod::IsPlaying(intptr_t instanceHandle)
    {
        auto iterator = m_Events.find(instanceHandle);

        if (iterator == m_Events.end())
        {
            Log::Warning("Attempted to check IsPlaying status using an invalid handle");
            return false;
        }

        FMOD::Studio::EventInstance* eventInstance = iterator->second;
        bool isPaused;
        eventInstance->getPaused(&isPaused);
        return !isPaused && GetPlaybackState(eventInstance) == FMOD_STUDIO_PLAYBACK_PLAYING;
    }

    bool LevFmod::IsStartingPlaying(intptr_t instanceHandle)
    {
        auto iterator = m_Events.find(instanceHandle);

        if (iterator == m_Events.end())
        {
            Log::Warning("Attempted to check IsStartingPlaying status using an invalid handle");
            return false;
        }

        FMOD::Studio::EventInstance* eventInstance = iterator->second;
        bool isPaused;
        eventInstance->getPaused(&isPaused);
        return !isPaused && GetPlaybackState(eventInstance) == FMOD_STUDIO_PLAYBACK_STARTING;
    }

    FMOD_STUDIO_PLAYBACK_STATE LevFmod::GetPlaybackState(FMOD::Studio::EventInstance* eventInstance)
    {
        if (eventInstance == nullptr)
        {
            return FMOD_STUDIO_PLAYBACK_STATE::FMOD_STUDIO_PLAYBACK_STOPPED;
        }

        FMOD_STUDIO_PLAYBACK_STATE state;
        eventInstance->getPlaybackState(&state);
        return state;
    }
#pragma endregion

    LevFmod::EventInfo* LevFmod::GetEventInfo(FMOD::Studio::EventInstance* eventInstance)
    {
        EventInfo* eventInfo;
        eventInstance->getUserData((void**)&eventInfo);
        return eventInfo;
    }

#pragma region Banks
    void LevFmod::LoadBank(const String& pathToBank, bool preloadSampleData)
    {
        if (IsBankRegistered(pathToBank))
        {
            return;
        }

        FMOD::Studio::Bank* bank = nullptr;
        CheckErrors(m_System->loadBankFile(pathToBank.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank));
        if (bank)
        {
            if (preloadSampleData)
            {
                bank->loadSampleData();
            }
            
            m_Banks.insert({ pathToBank, bank });
        }
        return;
    }

    void LevFmod::UnloadBank(const String& pathToBank)
    {
        if (!IsBankRegistered(pathToBank))
        {
            return;
        }

        auto keyValuePair = m_Banks.find(pathToBank);
        if (keyValuePair->second != nullptr) 
        {
            CheckErrors(keyValuePair->second->unload());
            m_Banks.erase(pathToBank);
        }
    }

    void LevFmod::UnloadAllBanks()
    {
        for (auto keyValuePair : m_Banks)
        {
            CheckErrors(keyValuePair.second->unload());
        }
        m_Banks.clear();
    }

    bool LevFmod::IsBankLoaded(const String& pathToBank)
    {
        if (!IsBankRegistered(pathToBank))
        {
            return false;
        }

        auto bank = m_Banks.find(pathToBank);
        if (bank->second != nullptr) 
        {
            FMOD_STUDIO_LOADING_STATE state;
            CheckErrors(bank->second->getLoadingState(&state));
            return state == FMOD_STUDIO_LOADING_STATE_LOADED;
        }
    }

    bool LevFmod::IsBankLoading(const String& pathToBank)
    {
        if (!IsBankRegistered(pathToBank))
        {
            return false;
        }

        auto bank = m_Banks.find(pathToBank);
        if (bank->second != nullptr)
        {
            FMOD_STUDIO_LOADING_STATE state;
            CheckErrors(bank->second->getLoadingState(&state));
            return state == FMOD_STUDIO_LOADING_STATE_LOADING;
        }
    }

    bool LevFmod::IsBankRegistered(const String pathToBank)
    {
        return m_Banks.count(pathToBank);
    }
#pragma endregion

#pragma region 3DAttributes
    auto LevFmod::Get3DAttributes(const FMOD_VECTOR pos,
        const FMOD_VECTOR up, FMOD_VECTOR forward, const FMOD_VECTOR vel) -> FMOD_3D_ATTRIBUTES
    {
        FMOD_3D_ATTRIBUTES f3d;
        f3d.forward = forward;
        f3d.position = pos;
        f3d.up = up;
        f3d.velocity = vel;
        return f3d;
    }

    FMOD_VECTOR LevFmod::ToFmodVector(const DirectX::SimpleMath::Vector3 vec)
    {
        FMOD_VECTOR fv;
        fv.x = vec.x;
        fv.y = vec.y;
        fv.z = vec.z;
        return fv;
    }

    FMOD_3D_ATTRIBUTES LevFmod::Get3DAttributes(const Transform& transform)
    {
        const Vector3 pos = transform.GetWorldPosition() / m_DistanceScale;
        const Vector3 up = transform.GetUpDirection();
        const Vector3 forward = transform.GetForwardDirection();
        constexpr Vector3 vel(0, 0, 0);

        return Get3DAttributes(ToFmodVector(pos), ToFmodVector(up),
            ToFmodVector(forward), ToFmodVector(vel));
    }

    void LevFmod::UpdateInstance3DAttributes(FMOD::Studio::EventInstance* instance, const Entity entity) {
        // try to set 3D attributes
        if (instance && entity && entity.HasComponent<Transform>())
        {
            const auto& transform = entity.GetComponent<Transform>();

            const FMOD_3D_ATTRIBUTES attr = Get3DAttributes(transform);
            CheckErrors(instance->set3DAttributes(&attr));
        }
    }
#pragma endregion

#pragma region Parameters
    float LevFmod::GetGlobalParameterByName(const String& parameterName) {
        float value;
        CheckErrors(m_System->getParameterByName(parameterName.c_str(), &value));
        return value;
    }

    void LevFmod::SetGlobalParameterByName(const String& parameterName, float value) {
        CheckErrors(m_System->setParameterByName(parameterName.c_str(), value));
    }

    float LevFmod::GetGlobalParameterByID(unsigned int idHalf1, unsigned int idHalf2) {
        FMOD_STUDIO_PARAMETER_ID id;
        id.data1 = idHalf1;
        id.data2 = idHalf2;
        float value;
        CheckErrors(m_System->getParameterByID(id, &value));
        return value;
    }

    void LevFmod::SetGlobalParameterByID(unsigned int idHalf1, unsigned int idHalf2, float value) {
        FMOD_STUDIO_PARAMETER_ID id;
        id.data1 = idHalf1;
        id.data2 = idHalf2;
        CheckErrors(m_System->setParameterByID(id, value));
    }

    float LevFmod::GetEventParameterByName(intptr_t instanceHandle, const String& parameterName) {
        auto iterator = m_Events.find(instanceHandle);

        if (iterator != m_Events.end())
        {
            float value;
            CheckErrors(iterator->second->getParameterByName(parameterName.c_str(), &value));
            return value;
        }
        else
        {
            return 0;
        }
    }

    void LevFmod::SetEventParameterByName(intptr_t instanceHandle, const String& parameterName, float value) {
        auto iterator = m_Events.find(instanceHandle);

        if (iterator != m_Events.end())
        {
            CheckErrors(iterator->second->setParameterByName(parameterName.c_str(), value));
        }
    }

    float LevFmod::GetEventParameterByID(intptr_t instanceHandle, unsigned int idHalf1, unsigned int idHalf2) {
        auto iterator = m_Events.find(instanceHandle);

        if (iterator != m_Events.end())
        {
            FMOD_STUDIO_PARAMETER_ID id;
            id.data1 = idHalf1;
            id.data2 = idHalf2;
            float value;
            CheckErrors(iterator->second->getParameterByID(id, &value));
            return value;
        }
        else
        {
            return 0;
        }
    }

    void LevFmod::SetEventParameterByID(intptr_t instanceHandle, unsigned int idHalf1, unsigned int idHalf2, float value) {
        auto iterator = m_Events.find(instanceHandle);

        if (iterator != m_Events.end())
        {
            FMOD_STUDIO_PARAMETER_ID id;
            id.data1 = idHalf1;
            id.data2 = idHalf2;
            CheckErrors(iterator->second->setParameterByID(id, value));
        }
    }
#pragma endregion

#pragma region EventPlayback
    void LevFmod::StopEventInstance(FMOD::Studio::EventInstance* eventInstance, bool forceImmediateStop)
    {
        FMOD_STUDIO_PLAYBACK_STATE state = GetPlaybackState(eventInstance);

        if (state != FMOD_STUDIO_PLAYBACK_STOPPED
            && state != FMOD_STUDIO_PLAYBACK_STOPPING)
        {
            CheckErrors(eventInstance->stop(forceImmediateStop
                ? FMOD_STUDIO_STOP_IMMEDIATE
                : FMOD_STUDIO_STOP_ALLOWFADEOUT));
        }
    }
#pragma endregion
    
#pragma region Event Lifetime
    FMOD::Studio::EventInstance* LevFmod::CreateInstance(String eventPath, bool isOneShot, Entity entity)
    {
        auto iterator = m_EventDescriptions.find(eventPath);
        FMOD::Studio::EventDescription* desc = nullptr;

        if (iterator == m_EventDescriptions.end()) 
        {
            if (!CheckErrors(m_System->getEvent(eventPath.c_str(), &desc)))
            {
                return 0;
            }

            m_EventDescriptions.insert({ eventPath, desc });
        }
        else
        {
            desc = iterator->second;
        }

        FMOD::Studio::EventInstance* instance;
        CheckErrors(desc->createInstance(&instance));

        if (instance && entity && !isOneShot) 
        {
            auto* eventInfo = new EventInfo();
            eventInfo->entity = entity;
            instance->setUserData(eventInfo);
            
            const auto instanceId = reinterpret_cast<intptr_t>(instance);
            m_Events[instanceId] = instance;
        }

        return instance;
    }

    FMOD::Studio::EventInstance* LevFmod::CreateInstance(const FMOD::Studio::EventDescription* eventDesc, bool isOneShot,
        Entity entity)
    {
        const auto desc = eventDesc;
        FMOD::Studio::EventInstance* instance;
        desc->createInstance(&instance);
        if (instance && (!isOneShot || entity)) 
        {
            auto* eventInfo = new EventInfo();
            eventInfo->entity = entity;
            instance->setUserData(eventInfo);
            const auto instanceId = reinterpret_cast<intptr_t>(instance);
            m_Events[instanceId] = instance;
        }

        return instance;
    }

    void LevFmod::ReleaseOneEvent(intptr_t instanceHandle, bool forceImmediateStop)
    {
        auto iterator = m_Events.find(instanceHandle);

        if (iterator == m_Events.end())
        {
            return;
        }

        FMOD::Studio::EventInstance* eventInstance = iterator->second;

        StopEventInstance(eventInstance, forceImmediateStop);

        eventInstance->setUserData(nullptr);
        m_Events.erase(instanceHandle);

        CheckErrors(eventInstance->release());
    }

    void LevFmod::ReleaseAllEvents()
    {
        for (auto keyValuePair : m_Events)
        {
            FMOD::Studio::EventInstance* eventInstance = keyValuePair.second;

            StopEventInstance(eventInstance, true);

            eventInstance->setUserData(nullptr);
            CheckErrors(eventInstance->release());
        }
        m_Events.clear();
        m_EventDescriptions.clear();
    }

    void LevFmod::ReleaseAll()
    {
        ReleaseAllEvents();
        UnloadAllBanks();
    }

    String LevFmod::GetPrefixedEventName(const String& eventName)
    {
        return eventName.find("event:/") != 0
            ? String("event:/") + eventName
            : eventName;
    }
#pragma endregion

}
