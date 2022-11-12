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

	class MidiMonitorScreen
		: public mpc::lcdgui::ScreenComponent
	{

	private:
		std::thread blinkThread;
		void runBlinkThread(std::weak_ptr<mpc::lcdgui::Label> label);
		static void static_blink(void* args, std::weak_ptr<mpc::lcdgui::Label> label);

	public:
		void initTimer(std::weak_ptr<mpc::lcdgui::Label> label);
		void update(moduru::observer::Observable* o, nonstd::any arg) override;

	public:
		MidiMonitorScreen(mpc::Mpc& mpc, const std::string& name, const int layerIndex);
		~MidiMonitorScreen();

		void open() override;
		void close() override;

	};
}
