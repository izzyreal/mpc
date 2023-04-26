#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{
	class SaveASequenceScreen
		: public mpc::lcdgui::ScreenComponent, public mpc::lcdgui::screens::OpensNameScreen
	{

	public:
		void turnWheel(int i) override;
		void function(int i) override;
        void openNameScreen() override;

		SaveASequenceScreen(mpc::Mpc& mpc, const int layerIndex);
		void open() override;

	private:
		void displaySaveAs();
		void displayFile();

		int saveSequenceAs = 1;
		void setSaveSequenceAs(int i);

	};
}
