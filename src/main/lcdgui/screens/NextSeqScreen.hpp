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

		NextSeqScreen(mpc::Mpc& mpc, const int layerIndex);
		
		void update(moduru::observer::Observable*, moduru::observer::Message) override;
		void open() override;
		void close() override;

	private:
		bool selectNextSqFromScratch = true;

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
