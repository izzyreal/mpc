#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
	class ZoneStartFineScreen;
	class ZoneEndFineScreen;
	class EditSoundScreen;
}

namespace mpc::sampler
{
	class Sampler;
}

namespace mpc::lcdgui::screens::window
{
	class NumberOfZonesScreen;
}

namespace mpc::lcdgui::screens
{

	class ZoneScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void openWindow() override;
		void function(int f) override;
		void turnWheel(int i) override;
		void pressEnter() override;
		void left() override;
		void right() override;
        void setSlider(int) override;

		ZoneScreen(mpc::Mpc& mpc, const int layerIndex);

		void open() override;
	
	private:
		void displayWave();
		void displaySnd();
		void displayPlayX();
		void displaySt();
		void displayEnd();
		void displayZone();

	private:
		const std::vector<std::string> playXNames{ "ALL", "ZONE", "BEFOR ST", "BEFOR TO", "AFTR END" };
		int numberOfZones = 16;
		std::vector<std::vector<int>> zones;
		int zone = 0;

		void initZones();
		void setZoneStart(int zoneIndex, int start);
		int getZoneStart(int zoneIndex);
		void setZoneEnd(int zoneIndex, int end);
		int getZoneEnd(int zoneIndex);
		void setZone(int i);
		std::vector<int> getZone();

		friend class mpc::lcdgui::screens::window::NumberOfZonesScreen;
		friend class mpc::lcdgui::screens::window::ZoneStartFineScreen;
		friend class mpc::lcdgui::screens::window::ZoneEndFineScreen;
		friend class mpc::lcdgui::screens::window::EditSoundScreen;
		friend class mpc::sampler::Sampler;

	};
}
