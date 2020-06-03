#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens {

	class OthersScreen
		: public mpc::lcdgui::ScreenComponent
	{

	private:
		void displayTapAveraging();
		int tapAveraging{ 2 };
		int contrast{ 10 };
		void setContrast(int i);

	public:
		void setTapAveraging(int i);
		int getTapAveraging();

		OthersScreen(const int layerIndex);
		void open() override;
		void function(int i) override;
		void turnWheel(int notch) override;

	};
}
