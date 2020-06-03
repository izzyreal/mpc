#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
	class NameScreen;
}

namespace mpc::lcdgui::screens::dialog
{
	class StereoToMonoScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void turnWheel(int i) override;
		void function(int i) override;

		StereoToMonoScreen(const int layerIndex);

		void open() override;

	private:
		void displayStereoSource();
		void displayNewLName();
		void displayNewRName();

		std::string newLName = "";
		std::string newRName = "";

		void setNewLName(std::string s);
		void setNewRName(std::string s);
		
	private:
		friend class mpc::lcdgui::screens::window::NameScreen;

	};
}
