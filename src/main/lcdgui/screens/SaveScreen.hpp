#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
	class SaveScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;

		SaveScreen(const int layerIndex);

		void open() override;

	private:
		const std::vector<std::string> types{ u8"Save All Sequences\u00CE&\u00CESongs", "Save a Sequence", u8"Save All Programs\u00CE&\u00CESounds", "Save a Program & Sounds", "Save a Sound", "Copy Operating System" };

		int type = 0;
		void setType(int i);

		void displayType();
		void displayFile();
		void displaySize();
		void displayFree();

	};
}
