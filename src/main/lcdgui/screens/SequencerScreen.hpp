#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <memory>
#include <vector>

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

		void checkTrackUsed();
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

	public:
		void update(moduru::observer::Observable* o, nonstd::any arg);

	public:
		void pressEnter() override;
		void function(int i) override;
		void turnWheel(int i) override;
		void openWindow() override;
		void left() override;
		void right() override;
		void up() override;
		void down() override;
		void numpad(int i) override { baseControls->numpad(i); }
		void play() override;
		void playStart() override;
		void stop() override;

	public:
		void open() override;
		void close() override;

		void setPunchRectOn(int i, bool b);
	};
}
