#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window {

	class EditMultipleScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;

	private:
		void checkThreeParameters();
		void checkFiveParameters();
		void checkNotes();

	public:
		EditMultipleScreen(const int& layer);

	};
}
