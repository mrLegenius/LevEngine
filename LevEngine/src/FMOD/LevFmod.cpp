#include "levpch.h"
#include "LevFmod.h"

#include "fmod_studio.hpp"
#include "Scene/Components/Audio/AudioListener.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    LevFmod::LevFmod()
    {
        m_Singleton = this;
        m_System = nullptr;
        m_CoreSystem = nullptr;
        CheckErrors(FMOD::Studio::System::create(&m_System));
        CheckErrors(m_System->getCoreSystem(&m_CoreSystem));
    }

    LevFmod::~LevFmod()
    {
        ReleaseAll();
        m_Singleton = nullptr;
    }

    void LevFmod::Init(int numOfChannels, int studioFlags, int flags) const
    {
        // initialize FMOD Studio and FMOD Core System with provided flags
        if (CheckErrors(m_System->initialize(numOfChannels, studioFlags, flags, nullptr))) {
            Log::CoreInfo("FMOD Sound System: Successfully initialized");
            if (studioFlags & FMOD_STUDIO_INIT_LIVEUPDATE)
                Log::CoreInfo("FMOD Sound System: Live update enabled!");
        }
        else
        {
            Log::CoreError("FMOD Sound System: Failed to initialize");
        }

        AudioListenerComponent unused;
    }

    void LevFmod::Update()
    {
        // clean up one shots
        for (auto it = m_Events.begin(); it != m_Events.end(); it++) {
            FMOD::Studio::EventInstance *eventInstance = it->second;
            const EventInfo *eventInfo = GetEventInfo(eventInstance);
            if (eventInfo->entity) {
                if (!eventInfo->entity.HasComponent<Transform>()) {
                    CheckErrors(eventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE));
                    ReleaseOneEvent(eventInstance);
                    continue;
                }
                UpdateInstance3DAttributes(eventInstance, eventInfo->entity);
            }
        }

        // update listener position
        SetListenerAttributes();

        // if events are subscribed to callbacks, update them
        //runCallbacks();

        // finally, dispatch an update call to FMOD
        CheckErrors(m_System->update());
    }

#pragma region Audio Listeners
    void LevFmod::AddListener(AudioListenerComponent* listenerComponent)
    {
        m_Listeners.emplace_back(listenerComponent);
    }

    void LevFmod::RemoveListener(AudioListenerComponent* listenerComponent)
    {
        m_Listeners.erase_first(listenerComponent);
    }

    void LevFmod::SetListenerAttributes()
    {
        if (m_Listeners.size() == 0) {
            if (m_ListenerWarning) {
                Log::Error("FMOD Sound System: No listeners are set!");
                m_ListenerWarning = false;
            }
            return;
        }

        for (int i = 0; i < m_Listeners.size(); i++) {
            const auto listener = m_Listeners[i];
            auto entity = listener->entity;

            if (!entity)
                continue;

            if (entity.HasComponent<Transform>())
            {
                if (!listener->listenerLock)
                {
                    Transform transform = entity.GetComponent<Transform>();
                    const auto attr = Get3DAttributes(transform);

                    CheckErrors(m_System->setListenerAttributes(i, &attr));
                }
            }
        }
    }
#pragma endregion

#pragma region One-Shot Sounds
    void LevFmod::PlayOneShot(const String& eventName, const Entity entity)
    {
        FMOD::Studio::EventInstance* instance = CreateInstance(eventName, true, entity);
        if (instance) {
            if (entity) {
                UpdateInstance3DAttributes(instance, entity);
            }
            CheckErrors(instance->start());
            CheckErrors(instance->release());
        }
    }
#pragma endregion

#pragma region Non-One-Shot Sounds
    FMOD::Studio::EventInstance* LevFmod::CreateSound(const String& eventName, Entity entity, bool playOnCreate)
    {
        FMOD::Studio::EventInstance* instance = CreateInstance(eventName, false, entity);
        if (instance) {
            if (entity) {
                UpdateInstance3DAttributes(instance, entity);
            }

            if (playOnCreate)
            {
                CheckErrors(instance->start());
            }

            return instance;
        }

        return nullptr;
    }

    void LevFmod::PlaySound(FMOD::Studio::EventInstance* instance)
    {
        CheckErrors(instance->start());
    }

    void LevFmod::StopSound(FMOD::Studio::EventInstance* instance, FMOD_STUDIO_STOP_MODE mode)
    {
        CheckErrors(instance->stop(mode));
    }

    void LevFmod::SetPause(FMOD::Studio::EventInstance* instance, bool isPaused)
    {
        CheckErrors(instance->setPaused(isPaused));
    }

    bool LevFmod::IsPlaying(FMOD::Studio::EventInstance* instance)
    {
        if (instance == nullptr)
        {
            return false;
        }

        FMOD_STUDIO_PLAYBACK_STATE state;
        instance->getPlaybackState(&state);
        return state == FMOD_STUDIO_PLAYBACK_STATE::FMOD_STUDIO_PLAYBACK_PLAYING;
    }
#pragma endregion

    LevFmod::EventInfo* LevFmod::GetEventInfo(FMOD::Studio::EventInstance* eventInstance)
    {
        EventInfo* eventInfo;
        eventInstance->getUserData((void**)&eventInfo);
        return eventInfo;
    }

#pragma region Banks
    void LevFmod::LoadBank(const String& pathToBank, int flags)
    {
        if (IsBankRegistered(pathToBank))
        {
            return;
        }

        FMOD::Studio::Bank* bank = nullptr;
        CheckErrors(m_System->loadBankFile(pathToBank.c_str(), flags, &bank));
        if (bank) {
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
        if (keyValuePair->second != nullptr) {
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

    FMOD_STUDIO_LOADING_STATE LevFmod::GetBankLoadingStatus(const String& pathToBank)
    {
        if (!IsBankRegistered(pathToBank))
        {
            return FMOD_STUDIO_LOADING_STATE::FMOD_STUDIO_LOADING_STATE_ERROR;
        }

        auto bank = m_Banks.find(pathToBank);
        if (bank->second != nullptr) {
            FMOD_STUDIO_LOADING_STATE state;
            CheckErrors(bank->second->getLoadingState(&state));
            return state;
        }

        return FMOD_STUDIO_LOADING_STATE::FMOD_STUDIO_LOADING_STATE_ERROR;
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
        if (instance && entity && entity.HasComponent<Transform>()) {
            const auto& transform = entity.GetComponent<Transform>();

            const FMOD_3D_ATTRIBUTES attr = Get3DAttributes(transform);
            CheckErrors(instance->set3DAttributes(&attr));
        }
    }
#pragma endregion

#pragma region Parameters


    float LevFmod::GetEventParameterByName(FMOD::Studio::EventInstance* eventInstance, const String& parameterName) {
        float value;
        CheckErrors(eventInstance->getParameterByName(parameterName.c_str(), &value));
        return value;
    }

    void LevFmod::SetEventParameterByName(FMOD::Studio::EventInstance* eventInstance, const String& parameterName, float value) {
        CheckErrors(eventInstance->setParameterByName(parameterName.c_str(), value));
    }

    float LevFmod::GetEventParameterByID(FMOD::Studio::EventInstance* eventInstance, unsigned int idHalf1, unsigned int idHalf2) {
        FMOD_STUDIO_PARAMETER_ID id;
        id.data1 = idHalf1;
        id.data2 = idHalf2;
        float value;
        CheckErrors(eventInstance->getParameterByID(id, &value));
        return value;
    }

    void LevFmod::SetEventParameterByID(FMOD::Studio::EventInstance* eventInstance, unsigned int idHalf1, unsigned int idHalf2, float value) {
        FMOD_STUDIO_PARAMETER_ID id;
        id.data1 = idHalf1;
        id.data2 = idHalf2;
        CheckErrors(eventInstance->setParameterByID(id, value));
    }
#pragma endregion
    
#pragma region Event Lifetime
    FMOD::Studio::EventInstance* LevFmod::CreateInstance(String eventPath, bool isOneShot, Entity entity)
    {
        if (!m_EventDescriptions.count(eventPath)) {
            FMOD::Studio::EventDescription* desc = nullptr;
            const auto res = CheckErrors(m_System->getEvent(eventPath.c_str(), &desc));
            if (!res) return 0;
            m_EventDescriptions.insert({ eventPath, desc });
        }
        const auto desc = m_EventDescriptions.find(eventPath)->second;
        FMOD::Studio::EventInstance* instance;
        CheckErrors(desc->createInstance(&instance));
        if (instance && (!isOneShot || entity)) {
            auto* eventInfo = new EventInfo();
            eventInfo->entity = entity;
            instance->setUserData(eventInfo);
            const auto instanceId = (uint64_t)instance;
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
        if (instance && (!isOneShot || entity)) {
            auto* eventInfo = new EventInfo();
            eventInfo->entity = entity;
            instance->setUserData(eventInfo);
            const auto instanceId = reinterpret_cast<uint64_t>(instance);
            m_Events[instanceId] = instance;
        }

        return instance;
    }

    void LevFmod::ReleaseOneEvent(FMOD::Studio::EventInstance* eventInstance)
    {
        if (eventInstance == nullptr)
        {
            return;
        }

        EventInfo* eventInfo = GetEventInfo(eventInstance);
        eventInstance->setUserData(nullptr);
        m_Events.erase((uint64_t)eventInstance);

        if (IsPlaying(eventInstance))
        {
            StopSound(eventInstance, FMOD_STUDIO_STOP_MODE::FMOD_STUDIO_STOP_IMMEDIATE);
        }

        CheckErrors(eventInstance->release());
        delete eventInfo;
    }

    void LevFmod::ReleaseAllEvents()
    {
        for (auto keyValuePair : m_Events)
        {
            FMOD::Studio::EventInstance* eventInstance = keyValuePair.second;
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
#pragma endregion

}
