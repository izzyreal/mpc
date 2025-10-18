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
		LoadASoundScreen(mpc::Mpc& mpc, const int layerIndex);

		void open() override;
		void close() override;

        void numpad(int i) override {}

		void update(Observable* observable, Message message) override;

		void turnWheel(int i) override;
		void function(int i) override;

	private:
		void displayAssignToNote();
        unsigned int assignToNote = 60;

	};
}
