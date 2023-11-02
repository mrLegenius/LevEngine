#pragma once
#include "Scene/Entity.h"

namespace LevEngine
{
	class Audio;
	class AudioBankAsset;

	class AudioPlayer
	{
	public:
		AudioPlayer();
		~AudioPlayer();

		void ReleaseSound();

		void Play(Entity audioSourceEntity = Entity());
		void Stop();


		bool IsPaused();
		void SetPaused(bool isPaused);

		[[nodiscard]] Ref<AudioBankAsset>& GetAudioBankAsset();
		void SetAudioBankAsset(Ref<AudioBankAsset> audioBank);

		[[nodiscard]] const String& GetEventName() const;
		void SetEventName(String eventName);

		[[nodiscard]] bool GetPlayOnInit() const;
		void SetPlayOnInit(bool playOnInit);

		[[nodiscard]] bool GetIsOneShot() const;
		void SetIsOneShot(bool isOneShot);

	private:
		void LoadBank(String& bankPath);

		intptr_t m_EventHandle;

		Ref<AudioBankAsset> m_AudioBank;
		String m_EventName;
		bool m_PlayOnInit{};
		bool m_IsOneShot{};
	};
}


