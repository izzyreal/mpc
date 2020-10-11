#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window {

	class TimeDisplayScreen
		: public mpc::lcdgui::ScreenComponent
	{
		
	public:
		void turnWheel(int i) override;

	public:
		TimeDisplayScreen(mpc::Mpc& mpc, const int layerIndex);

	public:
		void open() override;

	private:
		void displayDisplayStyle();
		void displayStartTime();
		void displayFrameRate();

	private:
		void setFrameRate(int i);
		void setDisplayStyle(int i);
		void setStartTime(int i);
		void setH(int i);
		void setM(int i);
		void setF(int i);
		void setS(int i);

	private:
		std::vector<std::string> displayStyleNames{ "BAR,BEAT,CLOCK", "HOUR,MINUTE,SEC" };
		std::vector<std::string> frameRateNames = { "24", "25", "30D", "30" };
		int displayStyle = 0;
		int startTime = 0;
		int frameRate = 0;
		int h = 0;
		int m = 0;
		int f = 0;
		int s = 0;

	};
}
