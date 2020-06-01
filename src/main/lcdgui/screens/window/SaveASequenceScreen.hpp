#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
	class SaveASequenceScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void turnWheel(int i) override;
		void function(int i) override;

		SaveASequenceScreen(const int layerIndex);
		void open() override;

	private:
		void displaySaveAs();
		void displayFile();

		int saveSequenceAs = 1;
		void setSaveSequenceAs(int i);

	};
}
