#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <memory>
#include <vector>
#include <functional>

namespace mpc::sequencer
{
	class Sequence;
	class Track;
}

namespace mpc::lcdgui::screens
{
	class SequencerScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		SequencerScreen(mpc::Mpc& mpc, const int layerIndex);

	private:
		std::weak_ptr<mpc::sequencer::Sequence> sequence;

		void setTrackToUsedIfItIsCurrentlyUnused();
		void displaySq();
		void displayTempoSource();
		void displayVelo();
		void displayDeviceNumber();
		void displayBus();
		void displayBars();
		void displayPgm();
		void displayDeviceName();
		void displayTempo();
		void displayTempoLabel();
		void displayOn();
		void displayTr();
		void displayCount();
		void displayTiming();

        // Moving the cursor is wrapped in order to handle some alternative
        // destination mapping while there is a next sequence queued up.
        void moveCursor(const std::function<void()>& cursorCall);

    public:
		// Shared with NextSeqObserver, find better home
		static std::vector<std::string> timingCorrectNames;

	private:
		static std::vector<std::string> busNames;
		void displayNow0();
		void displayNow1();
		void displayNow2();
		void displayRecordingMode();
		void displayTsig();
		void displayLoop();
		void displayPunchWhileRecording();
		void displayNextSq();

	public:
		void update(Observable* o, Message message) override;

		void pressEnter() override;
		void function(int i) override;
		void turnWheel(int i) override;
		void openWindow() override;
		void left() override;
		void right() override;
		void up() override;
		void down() override;
		void play() override;
		void playStart() override;
		void stop() override;
		void rec() override;
		void overDub() override;
        void erase() override;
        void tap() override;
        void shift() override;
        
		void open() override;
		void close() override;

        void setPunchRectOn(int i, bool b);
        
        void releaseErase();
        void releaseTap();

    };
}
