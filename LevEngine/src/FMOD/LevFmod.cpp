#include "levpch.h"
#include "LevFmod.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine::LevFmod
{
    LevFmod::LevFmod()
    {
        m_singleton = this;
        m_system = nullptr;
        m_coreSystem = nullptr;
        CheckErrors(FMOD::Studio::System::create(&m_system));
        CheckErrors(m_system->getCoreSystem(&m_coreSystem));
    }

    LevFmod::~LevFmod()
    {
        m_singleton = nullptr;
    }

    void LevFmod::Init(int numOfChannels, int studioFlags, int flags) const
    {
        // initialize FMOD Studio and FMOD Core System with provided flags
        if (CheckErrors(m_system->initialize(numOfChannels, studioFlags, flags, nullptr))) {
            Log::CoreInfo("FMOD Sound System: Successfully initialized");
            if (studioFlags & FMOD_STUDIO_INIT_LIVEUPDATE)
                Log::CoreInfo("FMOD Sound System: Live update enabled!");
        }
        else
        {
            Log::CoreError("FMOD Sound System: Failed to initialize");
        }
    }

    void LevFmod::Update()
    {
        // clean up one shots
        for (auto it = m_events.begin(); it != m_events.end(); it = it++) {
            FMOD::Studio::EventInstance *eventInstance = it->second;
            const EventInfo *eventInfo = GetEventInfo(eventInstance);
            if (eventInfo->entity) {
                if (!eventInfo->entity->HasComponent<Transform>()) {
                    CheckErrors(eventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE));
                    ReleaseOneEvent(eventInstance);
                    continue;
                }
                UpdateInstance3DAttributes(eventInstance, eventInfo->entity);
            }
        }

        // clean up invalid channel references
        ClearChannelRefs();

        // update listener position
        SetListenerAttributes();

        // if events are subscribed to callbacks, update them
        //runCallbacks();

        // finally, dispatch an update call to FMOD
        CheckErrors(m_system->update());
    }

    void LevFmod::ClearChannelRefs()
    {
        // not implemented
    }

    void LevFmod::SetListenerAttributes()
    {
        // not implemented
    }

    void LevFmod::PlayOneShot(const String& eventName, const Entity* entity)
    {
        FMOD::Studio::EventInstance *instance = CreateInstance(eventName, true, nullptr);
        if (instance) {
            // set 3D attributes once
            if (entity) {
                UpdateInstance3DAttributes(instance, entity);
            }
            CheckErrors(instance->start());
            CheckErrors(instance->release());
        }
    }

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

    void LevFmod::UpdateInstance3DAttributes(FMOD::Studio::EventInstance *instance, const Entity *entity) {
        // try to set 3D attributes
        if (instance && entity && entity->HasComponent<Transform>()) {
            const auto& transform = entity->GetComponent<Transform>();
            const Vector3 pos = transform.GetWorldPosition();
            const Vector3 up = transform.GetUpDirection();
            const Vector3 forward = transform.GetForwardDirection();
            constexpr Vector3 vel(0, 0, 0);

            const FMOD_3D_ATTRIBUTES attr = Get3DAttributes(ToFmodVector(pos), ToFmodVector(up),
                                                            ToFmodVector(forward), ToFmodVector(vel));
            
            CheckErrors(instance->set3DAttributes(&attr));
        }
    }

    LevFmod::EventInfo* LevFmod::GetEventInfo(FMOD::Studio::EventInstance* eventInstance)
    {
        EventInfo *eventInfo;
        eventInstance->getUserData((void **) &eventInfo);
        return eventInfo;
    }

    void LevFmod::ReleaseOneEvent(FMOD::Studio::EventInstance* eventInstance)
    {
        EventInfo *eventInfo = GetEventInfo(eventInstance);
        eventInstance->setUserData(nullptr);
        m_events.erase((uint64_t)eventInstance);
        CheckErrors(eventInstance->release());
        delete &eventInfo;
    }

    String LevFmod::LoadBank(const String& pathToBank, int flags)
    {
        if (m_banks.count(pathToBank)) return pathToBank; // bank is already loaded
        FMOD::Studio::Bank *bank = nullptr;
        CheckErrors(m_system->loadBankFile(pathToBank.c_str(), flags, &bank));
        if (bank) {
            m_banks.insert({pathToBank, bank});
            return pathToBank;
        }
        return pathToBank;
    }

    FMOD::Studio::EventInstance* LevFmod::CreateInstance(String eventPath, bool isOneShot, Entity* entity)
    {
        if (!m_eventDescriptions.count(eventPath)) {
            FMOD::Studio::EventDescription *desc = nullptr;
            const auto res = CheckErrors(m_system->getEvent(eventPath.c_str(), &desc));
            if (!res) return 0;
            m_eventDescriptions.insert({eventPath, desc});
        }
        const auto desc = m_eventDescriptions.find(eventPath)->second;
        FMOD::Studio::EventInstance *instance;
        CheckErrors(desc->createInstance(&instance));
        if (instance && (!isOneShot || entity)) {
            instance->setUserData(entity);
            const auto instanceId = (uint64_t)instance;
            m_events[instanceId] = instance;
        }
        return instance;
    }

    FMOD::Studio::EventInstance* LevFmod::CreateInstance(const FMOD::Studio::EventDescription* eventDesc, bool isOneShot,
                                                         Entity* entity)
    {
        const auto desc = eventDesc;
        FMOD::Studio::EventInstance *instance;
        desc->createInstance(&instance);
        if (instance && (!isOneShot || entity)) {
            instance->setUserData(entity);
            const auto instanceId = (uint64_t)instance;
            m_events[instanceId] = instance;
        }
        return instance;
    }
}
