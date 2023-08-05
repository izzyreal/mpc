#include "MidiMonitorScreen.hpp"

#include <Mpc.hpp>
#include <audiomidi/EventHandler.hpp>
#include <audiomidi/MidiInput.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::dialog;

MidiMonitorScreen::MidiMonitorScreen(mpc::Mpc& mpc, const std::string& name, const int layerIndex)
	: ScreenComponent(mpc, name, layerIndex)
{	
}

void MidiMonitorScreen::open()
{
	if (name == "midi-output-monitor")
	{
		mpc.getEventHandler()->addObserver(this);
	}
	else if (name == "midi-input-monitor")
	{
		mpc.getMpcMidiInput(0)->addObserver(this);
		mpc.getMpcMidiInput(1)->addObserver(this);
	}
}

void MidiMonitorScreen::close()
{
	if (name == "midi-output-monitor")
	{
		mpc.getEventHandler()->deleteObserver(this);
	}
	else if (name == "midi-input-monitor")
	{
		mpc.getMpcMidiInput(0)->deleteObserver(this);
		mpc.getMpcMidiInput(1)->deleteObserver(this);
	}

	if (blinkThread.joinable())
		blinkThread.join();
}

void MidiMonitorScreen::static_blink(void * arg1, std::weak_ptr<mpc::lcdgui::Label> label)
{
	static_cast<MidiMonitorScreen*>(arg1)->runBlinkThread(label);
}

void MidiMonitorScreen::runBlinkThread(std::weak_ptr<mpc::lcdgui::Label> label)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	label.lock()->setText(" ");
}

void MidiMonitorScreen::initTimer(std::weak_ptr<mpc::lcdgui::Label> label)
{
	if (blinkThread.joinable())
		blinkThread.join();

	blinkThread = std::thread(&MidiMonitorScreen::static_blink, this, label);
}

void MidiMonitorScreen::update(moduru::observer::Observable* o, moduru::observer::Message message)
{
    const auto msg = std::get<std::string>(message);

	int deviceNumber = stoi(msg.substr(1));
	
	if (msg[0] == 'b')
		deviceNumber += 16;
	
	auto label = findLabel(std::to_string(deviceNumber));
	label->setText(u8"\u00CC");
	initTimer(label);
}

MidiMonitorScreen::~MidiMonitorScreen()
{
	if (blinkThread.joinable())
		blinkThread.join();
}
