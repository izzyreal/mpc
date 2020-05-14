#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include <lcdgui/Component.hpp>

#include <sequencer/Sequencer.hpp>
#include <sampler/Sampler.hpp>

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
		SequencerScreen(const int& layer);

	private:
		mpc::sequencer::Sequencer& sequencer;
		mpc::sampler::Sampler& sampler;
		std::weak_ptr<mpc::sequencer::Sequence> sequence;
		std::weak_ptr<mpc::sequencer::Track> track;

	private:
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

	public:
		void open() override;
		void close() override;
	};
}
