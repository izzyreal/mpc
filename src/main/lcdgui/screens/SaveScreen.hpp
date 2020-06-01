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
		const std::vector<std::string> types{ "Save All Sequences & Songs", "Save a Sequence", "Save All Program and Sounds", "Save a Program & Sounds", "Save a Sound", "Copy Operating System" };

		int type = 0;
		void setType(int i);

		void displayType();
		void displayFile();
		void displaySize();
		void displayFree();

	};
}
