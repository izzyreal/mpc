#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <memory>

namespace mpc::ui::sampler::window
{
	class ZoomObserver;
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

	public:
		TrimScreen(const int layerIndex);
		void open() override;

	private:
		std::vector<std::string> playXNames = { "ALL", "ZONE", "BEFOR ST", "BEFOR TO", "AFTR END" };
		void displaySnd();
		void displayPlayX();
		void displaySt();
		void displayEnd();
		void displayView();
		void displayWave();

		int view = 0;
		void setView(int i);

	private:
		friend class LoopScreen;
		friend class ZoneScreen;
		friend class mpc::ui::sampler::window::ZoomObserver;

	};
}
