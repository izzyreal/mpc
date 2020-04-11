#include <ui/sampler/SampleObserver.hpp>

#include <Mpc.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <sampler/Sampler.hpp>

#include <file/File.hpp>

using namespace mpc::ui::sampler;
using namespace std;

SampleObserver::SampleObserver(mpc::Mpc* mpc) 
{
	samplerGui = mpc->getUis().lock()->getSamplerGui();
	samplerGui->addObserver(this);

	auto ls = mpc->getLayeredScreen().lock();
	inputField = ls->lookupField("input");
	thresholdField = ls->lookupField("threshold");
	modeField = ls->lookupField("mode");
	timeField = ls->lookupField("time");
	monitorField = ls->lookupField("monitor");
	preRecField = ls->lookupField("prerec");
	vuLeftLabel = ls->lookupLabel("vuleft");
	vuRightLabel = ls->lookupLabel("vuright");

	displayInput();
	displayThreshold();
	displayMode();
	displayTime();
	displayMonitor();
	displayPreRec();
}

void SampleObserver::displayInput()
{
    inputField.lock()->setText(inputNames[samplerGui->getInput()]);
}

void SampleObserver::displayThreshold()
{
	auto threshold = samplerGui->getThreshold() == -64 ? "-\u00D9\u00DA" : to_string(samplerGui->getThreshold());
	thresholdField.lock()->setText(threshold);
}

void SampleObserver::displayMode()
{
	modeField.lock()->setText(modeNames[samplerGui->getMode()]);
}

void SampleObserver::displayTime()
{
	string time = to_string(samplerGui->getTime());
	time = time.substr(0, time.length() - 1) + "." + time.substr(time.length() - 1);
	timeField.lock()->setText(time);
}

void SampleObserver::displayMonitor()
{
	monitorField.lock()->setText(monitorNames[samplerGui->getMonitor()]);
}

void SampleObserver::displayPreRec()
{
    preRecField.lock()->setText(to_string(samplerGui->getPreRec()) + "ms");
}

void SampleObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);
	if (s.compare("input") == 0) {
		displayInput();
	}
	else if (s.compare("threshold") == 0) {
		displayThreshold();
	}
	else if (s.compare("mode") == 0) {
		displayMode();
	}
	else if (s.compare("time") == 0) {
		displayTime();
	}
	else if (s.compare("monitor") == 0) {
		displayMonitor();
	}
	else if (s.compare("prerec") == 0) {
		displayPreRec();
	}
}

SampleObserver::~SampleObserver() {
	samplerGui->deleteObserver(this);
}
