#pragma once
#include <audio/server/AudioClient.hpp>
#include <sequencer/Clock.hpp>

#include <memory>
#include <vector>
#include <functional>
#include <atomic>

struct EventAfterNFrames {
    std::atomic<bool> occupied = ATOMIC_VAR_INIT(false);
    std::function<void()> f = [](){};
    unsigned long nFrames = 0;
    unsigned long frameCounter = -1;

    void init(const unsigned long& newNFrames, std::function<void()> callback) {
        occupied.store(true);
        nFrames = newNFrames;
        f = callback;
    }

    void reset() {
        f = [](){};
        nFrames = 0;
        frameCounter = -1;
        occupied.store(false);
    }
};

namespace mpc { class Mpc; }

namespace mpc::sequencer {

	class Sequencer;

	class FrameSeq final
		: public ctoot::audio::server::AudioClient
	{

	private:
		mpc::Mpc& mpc;
		int frameCounter = 0;
		bool running = false;
		bool metronome = false;
		Clock clock;
		std::shared_ptr<Sequencer> sequencer;
		int tickFrameOffset = 0;

		void move(int newTickPos);
		void repeatPad(int duration);
		void checkNextSq();
        void triggerClickIfNeeded();

	public:
        std::vector<EventAfterNFrames> eventsAfterNFrames = std::vector<EventAfterNFrames>(10);
		void start(float sampleRate);
		void startMetronome(int sampleRate);
		void work(int nFrames) override;
		void setEnabled(bool) override {};
		int getEventFrameOffset();
		void stop();
		bool isRunning();
		int getTickPosition();

	public:
		FrameSeq(mpc::Mpc& mpc);

	};
}
