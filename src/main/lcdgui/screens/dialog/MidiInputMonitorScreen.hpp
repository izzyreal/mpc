#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <thread>
#include <memory>

namespace mpc::lcdgui
{
	class Label;
}

namespace mpc::lcdgui::screens::dialog
{

	class MidiInputMonitorScreen
		: public mpc::lcdgui::ScreenComponent
	{

	private:
		std::thread blinkThread;
		void runBlinkThread(std::weak_ptr<mpc::lcdgui::Label> label);
		static void static_blink(void* args, std::weak_ptr<mpc::lcdgui::Label> label);

	public:
		void initTimer(std::weak_ptr<mpc::lcdgui::Label> label);
		void update(Observable* o, Message message) override;

	public:
		MidiInputMonitorScreen(mpc::Mpc& mpc, const int layerIndex);
		~MidiInputMonitorScreen();

		void open() override;
		void close() override;

	};
}
