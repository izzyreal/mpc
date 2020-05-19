#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
	class BarCopyScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int j) override;
		void turnWheel(int i) override;

		BarCopyScreen(const int& layer);

		void open() override;

	private:
		void displayCopies();
		void displayToSq();
		void displayFromSq();
		void displayAfterBar();
		void displayLastBar();
		void displayFirstBar();

	};
}
