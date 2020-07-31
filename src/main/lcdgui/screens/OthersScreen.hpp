#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <observer/Observable.hpp>

namespace mpc::lcdgui::screens {

	class OthersScreen
		: public mpc::lcdgui::ScreenComponent, public moduru::observer::Observable
	{

	private:
		void displayTapAveraging();
		int tapAveraging = 2;
		int contrast = 0;
		void setContrast(int i);

	public:
		void setTapAveraging(int i);
		int getTapAveraging();
		int getContrast();

		OthersScreen(mpc::Mpc& mpc, const int layerIndex);
		void open() override;
		void function(int i) override;
		void turnWheel(int notch) override;

	};
}
