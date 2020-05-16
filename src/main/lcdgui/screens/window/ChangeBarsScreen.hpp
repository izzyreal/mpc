#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window {

	class ChangeBarsScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;

	public:
		void open() override;

	private:
		void displayLastBar();
		void displayFirstBar();
		void displayNumberOfBars();
		void displayAfterBar();

	public:
		ChangeBarsScreen(const int& layer);

	private:
		int lastBar = 0;
		int firstBar = 0;
		int numberOfBars = 0;
		int afterBar = 0;

	public:
		int getLastBar();
		void setLastBar(int i, int max);
		int getFirstBar();
		void setFirstBar(int i, int max);
		int getNumberOfBars();
		void setNumberOfBars(int i, int max);
		int getAfterBar();
		void setAfterBar(int i, int max);

	};
}
