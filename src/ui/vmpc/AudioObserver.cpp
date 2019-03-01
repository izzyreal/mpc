#include <ui/vmpc/AudioObserver.hpp>

#include <Mpc.hpp>
#include <audiomidi/AudioMidiServices.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Field.hpp>
#include <ui/vmpc/AudioGui.hpp>

using namespace mpc::ui::vmpc;
using namespace std;

AudioObserver::AudioObserver(mpc::Mpc* mpc)
{
	ams = mpc->getAudioMidiServices();
	audioGui = mpc->getUis().lock()->getAudioGui();
	audioGui->deleteObservers();
	audioGui->addObserver(this);
	auto ls = mpc->getLayeredScreen().lock();
	driverField = ls->lookupField("server");
	dev0Field = ls ->lookupField("dev0");
	dev1Field = ls->lookupField("dev1");
	dev0Field.lock()->setScrolling(true);
	dev1Field.lock()->setScrolling(true);
	inField = ls->lookupField("in");
	outField = ls->lookupField("out");
	displayServer();
	displayIn();
	displayDev0();
	displayOut();
	displayDev1();
	displayFunctionKeys();
}

vector<string> AudioObserver::inNames{ "RECORD IN", "DIGITAL IN" };
vector<string> AudioObserver::outNames{ "STEREO OUT", "ASSIGNABLE MIX OUT 1/2", "ASSIGNABLE MIX OUT 3/4", "ASSIGNABLE MIX OUT 5/6", "ASSIGNABLE MIX OUT 7/8" };

void AudioObserver::displayServer()
{
	//auto candidate = ams.lock()->getServerName(audioGui->getServer());
	//driverField.lock()->setText(candidate);
}


void AudioObserver::displayIn()
{
	inField.lock()->setText(inNames[audioGui->getIn()]);
}

void AudioObserver::displayDev0()
{
	auto lAms = ams.lock();
	/*
	if (lAms->getServerIndex() != audioGui->getServer()) {
		dev0Field.lock()->setText(" <press F6 to activate server>");
	}
	else {
			string candidate = "<disabled>";
			auto devIndex = audioGui->getDev0();
			if (devIndex != -1) {
				candidate = lAms->getInputNames()[devIndex];
			}
			dev0Field.lock()->setText(candidate);
	}
	*/
	//dev0Field->enableScrolling(vector<mpc::lcdgui::Field*>{ dev0Field, inField });
}

void AudioObserver::displayOut()
{
	outField.lock()->setText(outNames[audioGui->getOut()]);
}

void AudioObserver::displayDev1()
{
	auto lAms = ams.lock();
	/*
	if (lAms->getServerIndex() != audioGui->getServer()) {
		dev1Field.lock()->setText(" <press F6 to activate server>");
	}
	else {
		string candidate = "<disabled>";
		int devIndex = audioGui->getDev1();
		if (devIndex != -1) {
			candidate = lAms->getOutputNames()[devIndex];
		}
		dev1Field.lock()->setText(candidate);
	}
	*/
	//dev0Field->enableScrolling(vector<mpc::lcdgui::Field*>{ dev0Field, inField });
}

void AudioObserver::update(moduru::observer::Observable* o, std::any a)
{

	string param = std::any_cast<string>(a);

	if (param.compare("server") == 0) {
		displayServer();
		displayIn();
		displayDev0();
		displayOut();
		displayDev1();
		displayFunctionKeys();
	}
	else if (param.compare("in") == 0) {
		displayIn();
		displayDev0();
	}
	else if (param.compare("out") == 0) {
		displayOut();
		displayDev1();
	}
	else if (param.compare("dev0") == 0) {
		displayDev0();
	}
	else if (param.compare("dev1") == 0) {
		displayDev1();
	}
}

void AudioObserver::displayFunctionKeys()
{
	/*
	if (ams.lock()->getServerIndex() == audioGui->getServer()) {
		//mainFrame->getLayeredScreen().lock()->drawFunctionKeys("audio_no_start");
	}
	else {
		//mainFrame->getLayeredScreen().lock()->drawFunctionKeys("audio");
	}
	*/
}

AudioObserver::~AudioObserver() {
	audioGui->deleteObserver(this);
	dev0Field.lock()->setScrolling(false);
	dev1Field.lock()->setScrolling(false);
}
