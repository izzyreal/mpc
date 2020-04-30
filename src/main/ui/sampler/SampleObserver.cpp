#include <ui/sampler/SampleObserver.hpp>

#include <Mpc.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/SoundRecorder.hpp>

#include <file/File.hpp>

using namespace mpc::ui::sampler;
using namespace std;

SampleObserver::SampleObserver() 
{
	

	samplerGui = Mpc::instance().getUis().lock()->getSamplerGui();
	samplerGui->addObserver(this);

	auto ams = Mpc::instance().getAudioMidiServices().lock();	
	ams->getSoundRecorder().lock()->addObserver(this);

	auto ls = Mpc::instance().getLayeredScreen().lock();
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
	if (dynamic_cast<mpc::audiomidi::SoundRecorder*>(o) != nullptr) {
		try {
			auto vuValue = nonstd::any_cast<float>(arg);
			updateVU(vuValue * 100);
		}
		catch (const std::exception& e) {
			// nothing to do
		}
		return;
	}

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

void SampleObserver::updateVU(float value)
{
	string lString = "";
	string rString = "";
	auto peaklValue = value;
	auto peakrValue = value;
	int thresholdValue = (samplerGui->getThreshold() + 63) * 0.53125;
	int levell = value;
	int levelr = value;
	for (int i = 0; i < 34; i++) {
		string l = " ";
		string r = " ";
		bool normall = i <= levell;
		bool normalr = i <= levelr;
		bool threshold = i == thresholdValue;
		bool peakl = i == peaklValue;
		bool peakr = i == peakrValue;
		if (threshold && peakl) l = vu_peak_threshold;
		if (threshold && peakr) r = vu_peak_threshold;

		if (threshold && normall && !peakl) l = vu_normal_threshold;
		if (threshold && normalr && !peakr) r = vu_normal_threshold;

		if (threshold && !peakl && !normall) l = vu_threshold;
		if (threshold && !peakr && !normalr) r = vu_threshold;

		if (normall && !peakl && !threshold) l = vu_normal;
		if (normalr && !peakr && !threshold) r = vu_normal;

		if (peakl && !threshold) l = vu_peak;
		if (peakr && !threshold) r = vu_peak;

		if (peakl && threshold && levell == 33) l = vu_peak_threshold_normal;
		if (peakr && threshold && levelr == 33) r = vu_peak_threshold_normal;

		lString += l;
		rString += r;
	}

	vuLeftLabel.lock()->setText(lString);
	vuRightLabel.lock()->setText(lString);
}

SampleObserver::~SampleObserver() {
	samplerGui->deleteObserver(this);
	auto ams = Mpc::instance().getAudioMidiServices().lock();
	ams->getSoundRecorder().lock()->deleteObserver(this);
}
