#pragma once
#include <audio/server/AudioClient.hpp>
#include <sequencer/Clock.hpp>

#include <memory>

namespace mpc {
	class Mpc;
	namespace sequencer {

		class Sequencer;

		class FrameSeq
			: public ctoot::audio::server::AudioClient
		{

		private:
			mpc::Mpc* mpc;
			int frameCounter = 0;
			bool running{ false };
			bool metronome{ false };
			Clock clock;
			std::weak_ptr<Sequencer> sequencer{};
			int tickFrameOffset{ 0 };
			
			void move(int newTickPos);
			void repeatPad(int tick);
			void checkNextSq();

		public:
			void start(float sampleRate);
			void startMetronome(int sampleRate);
			void work(int nFrames) override;
			void setEnabled(bool b) override {};
			int getEventFrameOffset(int tick);
			void stop();
			bool isRunning();
			int getTickPosition();

		public:
			FrameSeq(mpc::Mpc* mpc);
			~FrameSeq();

		};

	}
}
