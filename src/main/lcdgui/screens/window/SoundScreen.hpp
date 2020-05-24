#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
	class SoundScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void turnWheel(int i) override;
		void function(int i) override;

		SoundScreen(const int layerIndex);

		void open() override;

	private:
		void displaySoundName();
		void displayType();
		void displayRate();
		void displaySize();

	};
}
