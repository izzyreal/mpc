#pragma once

#include <lcdgui/ScreenComponent.hpp>

namespace mpc::audiomidi
{
	class EventHandler;
}

namespace mpc::lcdgui::screens::window
{
	class TransposePermanentScreen;
}

namespace mpc::lcdgui::screens
{
	class TransScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;

		void open() override;

		TransScreen(const int layerIndex);

	private:
		int tr = -1;
		int transposeAmount = 0;
		int bar0 = 0;
		int bar1 = 0;

		void setTransposeAmount(int i);
		void setTr(int i);
		void setBar0(int i);
		void setBar1(int i);

		void displayTransposeAmount();
		void displayTr();
		void displayBars();

		friend class mpc::lcdgui::screens::window::TransposePermanentScreen;
		friend class mpc::audiomidi::EventHandler;

	};
}
