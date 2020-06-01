#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
	class LoadASoundScreen
		: public mpc::lcdgui::ScreenComponent
	{

	private:
		void keepSound();

	public:
		LoadASoundScreen(const int layerIndex);

		void open() override;

		void turnWheel(int i) override;
		void function(int i) override;

	private:
		void displayAssignToNote();

	};
}
