#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{
	class NameScreen;
}

namespace mpc::lcdgui::screens::window
{
	class AutoChromaticAssignmentScreen
		: public mpc::lcdgui::ScreenComponent, public mpc::lcdgui::screens::OpensNameScreen
	{

	public:
		void turnWheel(int i) override;
		void function(int i) override;
        void openNameScreen() override;

		AutoChromaticAssignmentScreen(mpc::Mpc& mpc, const int layerIndex);
		void open() override;
		void close() override;
		void update(moduru::observer::Observable* o, nonstd::any arg) override;

	private:
		const std::vector<std::string> letters{ "A" , "B", "C", "D" };
		std::string newName;
		int sourceSoundIndex = -1;
		int originalKey = 67;
		int tune = 0;
		void setSourceSoundIndex(int i);
		void setOriginalKey(int i);
		void setTune(int i);

		void displayOriginalKey();
		void displaySource();
		void displayTune();
		void displayProgramName();
		void displaySnd();

	private:
		friend class mpc::lcdgui::screens::window::NameScreen;

	};
}
