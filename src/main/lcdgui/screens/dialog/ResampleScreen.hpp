#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
	class NameScreen;
}

namespace mpc::lcdgui::screens::dialog
{
	class ResampleScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void turnWheel(int newName2) override;
		void function(int i) override;

		ResampleScreen(mpc::Mpc& mpc, const int layerIndex);

		void open() override;

	private:
		const std::vector<std::string> qualityNames = std::vector<std::string>{ "LOW", "MED", "HIGH" };
		const std::vector<std::string> bitNames = std::vector<std::string>{ "16", "12", "8" };
		
		void displayNewFs();
		void displayQuality();
		void displayNewBit();
		void displayNewName();

		void setNewFs(int i);
		void setQuality(int i);
		void setNewBit(int i);
		void setNewName(std::string s);
		
		std::string newName = "";
		int newFs = 44100;
		int quality = 0;
		int newBit = 0;

	private:
		friend class mpc::lcdgui::screens::window::NameScreen;

	};
}
