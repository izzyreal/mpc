#pragma once
#include <audio/core/AudioBuffer.hpp>
#include <audio/core/AudioProcess.hpp>
#include <audio/core/AudioProcessAdapter.hpp>

namespace mpc {
	namespace ctootextensions {

		class MpcInputAdapter
			: public ctoot::audio::core::AudioProcessAdapter
		{

		public:
			typedef ctoot::audio::core::AudioProcessAdapter super;

		private:
			ctoot::audio::core::AudioProcess* audioProcess{};

		public:

			int processAudio(ctoot::audio::core::AudioBuffer* buf) override;
			virtual ctoot::audio::core::AudioProcess* getProcess();

			MpcInputAdapter(ctoot::audio::core::AudioProcess* process);

		};

	};
};