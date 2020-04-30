#include "MidiMonitorObserver.hpp"

#include <Mpc.hpp>
#include <audiomidi/EventHandler.hpp>
#include <lcdgui/Label.hpp>

using namespace mpc::ui::sequencer::window;
using namespace std;

MidiMonitorObserver::MidiMonitorObserver() 
{
	
	auto lEventHandler = Mpc::instance().getEventHandler().lock();
	lEventHandler->addObserver(this);
	auto ls = Mpc::instance().getLayeredScreen().lock();
	a0 = ls->lookupLabel("0");
	a1 = ls->lookupLabel("1");
	a2 = ls->lookupLabel("2");
	a3 = ls->lookupLabel("3");
	a4 = ls->lookupLabel("4");
	a5 = ls->lookupLabel("5");
	a6 = ls->lookupLabel("6");
	a7 = ls->lookupLabel("7");
	a8 = ls->lookupLabel("8");
	a9 = ls->lookupLabel("9");
	a10 = ls->lookupLabel("10");
	a11 = ls->lookupLabel("11");
	a12 = ls->lookupLabel("12");
	a13 = ls->lookupLabel("13");
	a14 = ls->lookupLabel("14");
	a15 = ls->lookupLabel("15");
	b0 = ls->lookupLabel("16");
	b1 = ls->lookupLabel("17");
	b2 = ls->lookupLabel("18");
	b3 = ls->lookupLabel("19");
	b4 = ls->lookupLabel("20");
	b5 = ls->lookupLabel("21");
	b6 = ls->lookupLabel("22");
	b7 = ls->lookupLabel("23");
	b8 = ls->lookupLabel("24");
	b9 = ls->lookupLabel("25");
	b10 = ls->lookupLabel("26");
	b11 = ls->lookupLabel("27");
	b12 = ls->lookupLabel("28");
	b13 = ls->lookupLabel("29");
	b14 = ls->lookupLabel("30");
	b15 = ls->lookupLabel("31");
}

void MidiMonitorObserver::static_blink(void * arg1, weak_ptr<mpc::lcdgui::Label> label)
{
	static_cast<MidiMonitorObserver*>(arg1)->runBlinkThread(label);
}

void MidiMonitorObserver::runBlinkThread(weak_ptr<mpc::lcdgui::Label> label) {
	this_thread::sleep_for(chrono::milliseconds(50));
	label.lock()->setText("");
}

void MidiMonitorObserver::initTimer(std::weak_ptr<mpc::lcdgui::Label> label)
{
	thread* foo = new thread(&MidiMonitorObserver::static_blink, this, label);
}

void MidiMonitorObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);
	int deviceNumber = stoi(s.substr(1));
	if (s[0] == 'b') deviceNumber += 16;
	auto label = Mpc::instance().getLayeredScreen().lock()->lookupLabel(to_string(deviceNumber));
	label.lock()->setText(u8"\u00CC");
	initTimer(label);
}

MidiMonitorObserver::~MidiMonitorObserver() {
	Mpc::instance().getEventHandler().lock()->deleteObserver(this);
}
