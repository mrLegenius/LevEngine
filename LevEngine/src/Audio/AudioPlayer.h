#pragma once
#include "Scene/Entity.h"

namespace LevEngine
{
	class LevFmod;
	class AudioBankAsset;

	class AudioPlayer
	{
	public:
		AudioPlayer();
		~AudioPlayer();

		void ReleaseSound();

		void Play(Entity audioSourceEntity = Entity());
		void SetPaused(bool isPaused);
		void Stop();

		[[nodiscard]] Ref<AudioBankAsset>& GetAudioBankAsset();
		[[nodiscard]] const String& GetEventName() const;
		[[nodiscard]] bool GetPlayOnInit() const;
		[[nodiscard]] bool GetIsOneShot() const;

		void SetAudioBankAsset(Ref<AudioBankAsset> audioBank);
		void SetEventName(String eventName);
		void SetPlayOnInit(bool playOnInit);
		void SetIsOneShot(bool isOneShot);

	private:
		void LoadBank(String& bankPath);

		Ref<LevFmod> m_Fmod;
		intptr_t m_EventHandle;

		Ref<AudioBankAsset> m_AudioBank;
		String m_EventName;
		bool m_PlayOnInit{};
		bool m_IsOneShot{};
	};
}


