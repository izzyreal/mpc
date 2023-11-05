#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <memory>

namespace mpc::lcdgui::screens::window
{
	class StartFineScreen;
	class EndFineScreen;
	class LoopToFineScreen;
	class LoopEndFineScreen;
	class ZoneStartFineScreen;
	class ZoneEndFineScreen;
}

namespace mpc::lcdgui::screens
{
	class LoopScreen;
	class ZoneScreen;
}

namespace mpc::lcdgui::screens
{
	class TrimScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void openWindow() override;
		void function(int f) override;
		void turnWheel(int i) override;
		void left() override;
		void right() override;
		void pressEnter() override;
		void setSlider(int i) override;

        void setSliderStart(int);
        void setSliderEnd(int);

	public:
		TrimScreen(mpc::Mpc& mpc, const int layerIndex);
		void open() override;

	private:
		const std::vector<std::string> playXNames = { "ALL", "ZONE", "BEFOR ST", "BEFOR TO", "AFTR END" };
		void displaySnd();
		void displayPlayX();
		void displaySt();
		void displayEnd();
		void displayView();
		void displayWave();

		bool smplLngthFix = false;
		int view = 0;

		void setView(int i);

	private:
		friend class LoopScreen;
		friend class ZoneScreen;
		friend class mpc::lcdgui::screens::window::StartFineScreen;
		friend class mpc::lcdgui::screens::window::EndFineScreen;
		friend class mpc::lcdgui::screens::window::LoopEndFineScreen;
		friend class mpc::lcdgui::screens::window::LoopToFineScreen;
		friend class mpc::lcdgui::screens::window::ZoneStartFineScreen;
		friend class mpc::lcdgui::screens::window::ZoneEndFineScreen;

	};
}
