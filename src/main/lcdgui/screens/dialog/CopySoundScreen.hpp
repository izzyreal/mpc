#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{

	class CopySoundScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;

		CopySoundScreen(const int layerIndex);

		void open() override;

	private:
		void displaySnd();
		void displayNewName();

	};
}
