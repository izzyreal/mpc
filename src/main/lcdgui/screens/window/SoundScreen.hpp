#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{
	class SoundScreen
		: public mpc::lcdgui::ScreenComponent, public mpc::lcdgui::screens::OpensNameScreen
	{

	public:
		void function(int i) override;
        void openNameScreen() override;

		SoundScreen(mpc::Mpc& mpc, const int layerIndex);

		void open() override;

	private:
		void displaySoundName();
		void displayType();
		void displayRate();
		void displaySize();

	};
}
