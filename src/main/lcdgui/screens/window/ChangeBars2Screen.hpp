#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{

	class ChangeBars2Screen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;

	public:
		ChangeBars2Screen(const int& layer);

	public:
		void open() override;

	private:
		int newBars = 1;
		void setNewBars(int i);

	private:
		void displayCurrent();
		void displayNewBars();

	};
}
