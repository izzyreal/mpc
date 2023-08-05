#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include <lcdgui/screens/OpensNameScreen.hpp>

namespace mpc::lcdgui::screens::window
{
	class KeepOrRetryScreen
: public mpc::lcdgui::ScreenComponent, public mpc::lcdgui::screens::OpensNameScreen
	{

	public:
        KeepOrRetryScreen(mpc::Mpc& mpc, const int layerIndex);

        void update(moduru::observer::Observable* o, moduru::observer::Message message) override;

        void open() override;
        void close() override;
		void function(int i) override;
		void turnWheel(int i) override;
		void mainScreen() override;
        void right() override;

	private:
		void displayNameForNewSound();
		void displayAssignToNote();
        void openNameScreen() override;
        unsigned char assignToNote = 34;

	};
}
