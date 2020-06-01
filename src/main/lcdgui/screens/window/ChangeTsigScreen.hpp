#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <sequencer/TimeSignature.hpp>

namespace mpc::lcdgui::screens::window {

	class ChangeTsigScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;

	public:
		ChangeTsigScreen(const int layerIndex);

	public:
		void open() override;

	private:
		void displayBars();
		void displayNewTsig();

	private:
		mpc::sequencer::TimeSignature timesignature;
		int bar0 = 0;
		int bar1 = 0;

	private:
		void setBar0(int i, int max);
		void setBar1(int i, int max);
//		mpc::sequencer::TimeSignature& getNewTimeSignature();
	//	void setNewTimeSignature(const mpc::sequencer::TimeSignature& ts);

	};
}
