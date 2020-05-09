#pragma once

#include <lcdgui/ScreenComponent.hpp>
#include <lcdgui/Component.hpp>

#include <sequencer/Sequencer.hpp>
#include <sampler/Sampler.hpp>

#include <memory>
#include <vector>

namespace mpc::lcdgui::screens
{

	class SequencerScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		SequencerScreen(std::vector<std::shared_ptr<Component>> componentMap);

	private:
		mpc::sequencer::Sequencer& sequencer;
		mpc::sampler::Sampler& sampler;

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

	private:
		static std::vector<std::string> timingCorrectNames;
		static std::vector<std::string> busNames;
		void displayNow0();
		void displayNow1();
		void displayNow2();
		void displayRecordingMode();
		void displayTsig();
		void displayLoop();


	};

}