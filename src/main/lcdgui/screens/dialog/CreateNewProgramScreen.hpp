#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
	class NameScreen;
}

namespace mpc::lcdgui::screens::dialog
{
	class CreateNewProgramScreen
		: public mpc::lcdgui::ScreenComponent
	{
	public:
		void function(int i) override;

		CreateNewProgramScreen(mpc::Mpc& mpc, const int layerIndex);

		void open() override;
		void turnWheel(int i) override;

	private:
		std::string newName = "";
		int midiProgramChange = 0;

		void setMidiProgramChange(int i);

		void displayNewName();
		void displayMidiProgramChange();

	private:
		friend class mpc::lcdgui::screens::window::NameScreen;

	};
}
