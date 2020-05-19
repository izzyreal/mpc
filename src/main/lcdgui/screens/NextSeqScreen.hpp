#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
	class NextSeqScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void turnWheel(int i) override;
		void function(int i) override;

		NextSeqScreen(const int& layer);

		void open() override;

	private:
		void displaySq();
		void displayNextSq();
		void displayNow0();
		void displayNow1();
		void displayNow2();
		void displayTempo();
		void displayTempoLabel();
		void displayTempoSource();
		void displayTiming();
	};
}
