#pragma once

#include <audio/core/SimpleAudioProcess.hpp>

namespace mpc {
	namespace ctootextensions {

		class MpcMixerInterconnection;

		class InterconnectionInputProcess
			: public ctoot::audio::core::SimpleAudioProcess
		{

		public:
			int processAudio(ctoot::audio::core::AudioBuffer* buffer) override;

		private:
			MpcMixerInterconnection* mmi{ nullptr };
			ctoot::audio::core::AudioBuffer* sharedBuffer{ nullptr };

		public:
			InterconnectionInputProcess(MpcMixerInterconnection* mmi, ctoot::audio::core::AudioBuffer* sharedBuffer);
			~InterconnectionInputProcess();

		};
	}
}
