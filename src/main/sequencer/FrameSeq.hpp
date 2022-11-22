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

namespace ctoot::midi::core { class ShortMessage; }

namespace mpc { class Mpc; }
namespace mpc::lcdgui::screens::window { class CountMetronomeScreen; class TimingCorrectScreen; }
namespace mpc::lcdgui::screens { class SequencerScreen; class UserScreen; class SyncScreen; class PunchScreen; class SongScreen; }

namespace mpc::sequencer {

	class Sequencer;
    class Sequence;

	class FrameSeq final
		: public ctoot::audio::server::AudioClient
	{

	private:
        std::shared_ptr<mpc::lcdgui::screens::window::CountMetronomeScreen> countMetronomeScreen;
        std::shared_ptr<mpc::lcdgui::screens::window::TimingCorrectScreen> timingCorrectScreen;
        std::shared_ptr<mpc::lcdgui::screens::SequencerScreen> sequencerScreen;
        std::shared_ptr<mpc::lcdgui::screens::UserScreen> userScreen;
        std::shared_ptr<mpc::lcdgui::screens::SyncScreen> syncScreen;
        std::shared_ptr<mpc::lcdgui::screens::PunchScreen> punchScreen;
        std::shared_ptr<mpc::lcdgui::screens::SongScreen> songScreen;

        mpc::Mpc& mpc;
		int frameCounter = 0;
		bool running = false;
        bool metronome = false;
        Clock clock;
		std::shared_ptr<Sequencer> sequencer;
		int tickFrameOffset = 0;
        bool wasRunning = false;
        std::shared_ptr<ctoot::midi::core::ShortMessage> midiSyncClockMsg;
        std::shared_ptr<ctoot::midi::core::ShortMessage> midiSyncStartStopContinueMsg;
        std::vector<EventAfterNFrames> eventsAfterNFrames = std::vector<EventAfterNFrames>(100);

        void move(int newTickPos);
		void repeatPad(int duration);
		std::shared_ptr<mpc::sequencer::Sequence> switchToNextSequence();
        void triggerClickIfNeeded();

        void displayPunchRects();
        void stopCountingInIfRequired();
        bool processSongMode();
        bool processSeqLoopEnabled();
        void processSeqLoopDisabled();
        void processNoteRepeat();
        void updateTimeDisplay(unsigned int nFrames);
        void processTempoChange();
        void stopSequencer();

    public:
		void start(float sampleRate);
		void startMetronome(int sampleRate);
		void work(int nFrames) override;
		void setEnabled(bool) override {};
		int getEventFrameOffset();
		void stop();
		bool isRunning();
		int getTickPosition();

        void sendMidiSyncMsg(unsigned char status);
        void enqueEventAfterNFrames(std::function<void()>, unsigned long nFrames);

    public:
		FrameSeq(mpc::Mpc& mpc);

	};
}
