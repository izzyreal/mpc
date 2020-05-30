#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <controls/BaseSamplerControls.hpp>

namespace mpc::controls::other::dialog
{
	class NameControls;
}

namespace mpc::lcdgui::screens::dialog
{
	class CreateNewProgramScreen
		: public mpc::lcdgui::ScreenComponent, public mpc::controls::BaseSamplerControls
	{
	public:
		void function(int i) override;

		CreateNewProgramScreen(const int layerIndex);

		void open() override;

	private:
		std::string newName = "";
		int newProgramChange = 1;

		void displayNewName();
		void displayMidiProgramChange();

	private:
		friend class mpc::controls::other::dialog::NameControls;

	};
}
