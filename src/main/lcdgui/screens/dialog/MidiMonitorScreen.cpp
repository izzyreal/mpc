#include "MidiMonitorScreen.hpp"

#include <Mpc.hpp>
#include <audiomidi/EventHandler.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::dialog;
using namespace std;

MidiMonitorScreen::MidiMonitorScreen(const int layerIndex)
	: ScreenComponent("midi-monitor", layerIndex)
{	
}

void MidiMonitorScreen::open()
{
	a0 = findLabel("0");
	a1 = findLabel("1");
	a2 = findLabel("2");
	a3 = findLabel("3");
	a4 = findLabel("4");
	a5 = findLabel("5");
	a6 = findLabel("6");
	a7 = findLabel("7");
	a8 = findLabel("8");
	a9 = findLabel("9");
	a10 = findLabel("10");
	a11 = findLabel("11");
	a12 = findLabel("12");
	a13 = findLabel("13");
	a14 = findLabel("14");
	a15 = findLabel("15");
	b0 = findLabel("16");
	b1 = findLabel("17");
	b2 = findLabel("18");
	b3 = findLabel("19");
	b4 = findLabel("20");
	b5 = findLabel("21");
	b6 = findLabel("22");
	b7 = findLabel("23");
	b8 = findLabel("24");
	b9 = findLabel("25");
	b10 = findLabel("26");
	b11 = findLabel("27");
	b12 = findLabel("28");
	b13 = findLabel("29");
	b14 = findLabel("30");
	b15 = findLabel("31");

	mpc.getEventHandler().lock()->addObserver(this);
}

void MidiMonitorScreen::close()
{
	mpc.getEventHandler().lock()->deleteObserver(this);
	
	if (blinkThread.joinable())
	{
		blinkThread.join();
	}
}

void MidiMonitorScreen::static_blink(void * arg1, weak_ptr<mpc::lcdgui::Label> label)
{
	static_cast<MidiMonitorScreen*>(arg1)->runBlinkThread(label);
}

void MidiMonitorScreen::runBlinkThread(weak_ptr<mpc::lcdgui::Label> label) {
	this_thread::sleep_for(chrono::milliseconds(50));
	label.lock()->setText("");
}

void MidiMonitorScreen::initTimer(std::weak_ptr<mpc::lcdgui::Label> label)
{
	if (blinkThread.joinable())
	{
		blinkThread.join();
	}
	blinkThread = thread(&MidiMonitorScreen::static_blink, this, label);
}

void MidiMonitorScreen::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);
	int deviceNumber = stoi(s.substr(1));
	if (s[0] == 'b') deviceNumber += 16;
	auto label = findLabel(to_string(deviceNumber));
	label.lock()->setText(u8"\u00CC");
	initTimer(label);
}
