#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
	class NameScreen;
}

namespace mpc::lcdgui::screens::dialog
{

	class CopySoundScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;
		void turnWheel(int renamerNewName) override;

		CopySoundScreen(mpc::Mpc& mpc, const int layerIndex);

		void open() override;

	private:
		void displaySnd();
		void displayNewName();

		std::string newName;
		void setNewName(std::string s);

	private:
		friend class mpc::lcdgui::screens::window::NameScreen;

	};
}
