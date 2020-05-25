#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::controls::other::dialog
{
	class NameControls;
}

namespace mpc::lcdgui::screens::dialog
{
	class ResampleScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void turnWheel(int i) override;
		void function(int i) override;

		ResampleScreen(const int layerIndex);

		void open() override;

	private:
		const std::vector<std::string> qualityNames = vector<string>{ "LOW", "MED", "HIGH" };
		const std::vector<std::string> bitNames = vector<string>{ "16", "12", "8" };
		
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
		friend class mpc::controls::other::dialog::NameControls;

	};
}
