#pragma once
#include <audio/core/SimpleAudioProcess.hpp>

namespace mpc {
	namespace ctootextensions {

		class MpcMixerInterconnection;
		class InterconnectionOutputProcess
			: public ctoot::audio::core::SimpleAudioProcess
		{

		public:
			int processAudio(ctoot::audio::core::AudioBuffer* buffer) override;

		private:
			MpcMixerInterconnection* mmi{ nullptr };
			ctoot::audio::core::AudioBuffer* sharedBuffer{ nullptr };

		public:
			InterconnectionOutputProcess(MpcMixerInterconnection* mmi, ctoot::audio::core::AudioBuffer* sharedBuffer);
			~InterconnectionOutputProcess();
	
		};
	
	}
}
