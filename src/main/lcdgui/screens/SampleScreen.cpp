#include "SampleScreen.hpp"

#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/SoundRecorder.hpp>

#include <cmath>

using namespace mpc::lcdgui::screens;
using namespace std;

SampleScreen::SampleScreen(const int layerIndex)
	: ScreenComponent("sample", layerIndex)
{
}

void SampleScreen::open()
{
	displayInput();
	displayThreshold();
	displayMode();
	displayTime();
	displayMonitor();
	displayPreRec();

	auto ams = mpc.getAudioMidiServices().lock();
	ams->getSoundRecorder().lock()->addObserver(this);
}

void SampleScreen::close()
{
	auto ams = mpc.getAudioMidiServices().lock();
	ams->getSoundRecorder().lock()->deleteObserver(this);

}

void SampleScreen::left() {
	//if (sampler.lock()->isArmed() || sampler.lock()->isRecording()) return;
	if (mpc.getAudioMidiServices().lock()->isRecordingSound())
	{
		return;
	}
	BaseControls::left();
}

void SampleScreen::right() {
	//if (sampler.lock()->isArmed() || sampler.lock()->isRecording()) return;
	if (mpc.getAudioMidiServices().lock()->isRecordingSound()) {
		return;
	}
	BaseControls::right();
}

void SampleScreen::up() {
	//if (sampler.lock()->isArmed() || sampler.lock()->isRecording()) return;
	if (mpc.getAudioMidiServices().lock()->isRecordingSound()) {
		return;
	}
	BaseControls::up();
}

void SampleScreen::down() {
	//if (sampler.lock()->isArmed() || sampler.lock()->isRecording()) return;
	if (mpc.getAudioMidiServices().lock()->isRecordingSound()) {
		return;
	}
	BaseControls::down();
}

void SampleScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 0:
		if (mpc.getAudioMidiServices().lock()->isRecordingSound()) {
			return;
		}
		peakL = 0.f;
		peakR = 0.f;
		break;
	case 4:
		if (mpc.getAudioMidiServices().lock()->isRecordingSound()) {
			return;
		}
		//if (samplerisRecording()) {
		//	samplercancelRecording();
		//	return;
		//}
		//if (samplerisArmed()) {
		//	samplerunArm();
		//	return;
		//}
		break;
	case 5:
		if (mpc.getControls().lock()->isF6Pressed())
		{
			return;
		}

		mpc.getControls().lock()->setF6Pressed(true);

		auto ams = mpc.getAudioMidiServices().lock();

		if (ams->isRecordingSound())
		{
			ams->stopSoundRecorder();
			return;
		}

		auto sound = sampler.lock()->addSound();
		sound.lock()->setName(sampler.lock()->addOrIncreaseNumber("sound"));
		auto lengthInFrames = time * (44100 * 0.1);
		ams->getSoundRecorder().lock()->prepare(sound, lengthInFrames);
		ams->startRecordingSound();

		//if (!samplerisRecording()) {
			//samplerarm();
			//return;
		//}
		//if (samplerisRecording()) {
			//samplerstopRecordingEarlier();
			//return;
		//}
		break;
	}
}

void SampleScreen::turnWheel(int i)
{
    init();
	//if (!samplerisRecording() && !samplerisArmed()) {
        if (param.compare("input") == 0)
		{
            setInput(input + i);
        }
		else if (param.compare("threshold") == 0)
		{
			setThreshold(threshold + i);
		}
		else if (param.compare("mode") == 0)
		{
			setMode(mode + i);
		}
		else if (param.compare("time") == 0)
		{
			setTime(time + i);
		}
		else if (param.compare("monitor") == 0)
		{
			setMonitor(monitor + i);
			bool muteMonitor = monitor == 0;
			mpc.getAudioMidiServices().lock()->muteMonitor(muteMonitor);
		}
		else if (param.compare("prerec") == 0)
		{
			setPreRec(preRec + i);
		}
    //}
}


void SampleScreen::setInput(int i)
{
	if (i < 0 || i > 1)
	{
		return;
	}

	input = i;
	displayInput();
}

void SampleScreen::setThreshold(int i)
{
	if (i < -64 || i > 0)
	{
		return;
	}

	threshold = i;
	displayThreshold();
}

void SampleScreen::setMode(int i)
{
	if (i < 0 || i > 2)
	{
		return;
	}

	mode = i;
	displayMode();
}

void SampleScreen::setTime(int i)
{
	if (i < 0 || i > 3786)
	{
		return;
	}

	time = i;
	displayTime();
}

void SampleScreen::setMonitor(int i)
{
	if (i < 0 || i > 5)
	{
		return;
	}

	monitor = i;
	displayMonitor();
}

void SampleScreen::setPreRec(int i)
{
	if (i < 0 || i > 100)
	{
		return;
	}

	preRec = i;
	displayPreRec();
}

void SampleScreen::displayInput()
{
	findField("input").lock()->setText(inputNames[input]);
}

void SampleScreen::displayThreshold()
{
	auto thresholdText = threshold == -64 ? u8"-\u00D9\u00DA" : to_string(threshold);
	findField("threshold").lock()->setTextPadded(thresholdText);
}

void SampleScreen::displayMode()
{
	findField("mode").lock()->setText(modeNames[mode]);
}

void SampleScreen::displayTime()
{
	string timeText = to_string(time);
	timeText = timeText.substr(0, timeText.length() - 1) + "." + timeText.substr(timeText.length() - 1);
	findField("time").lock()->setTextPadded(timeText);
}

void SampleScreen::displayMonitor()
{
	findField("monitor").lock()->setText(monitorNames[monitor]);
}

void SampleScreen::displayPreRec()
{
	findField("prerec").lock()->setTextPadded(preRec);
}

void SampleScreen::updateVU(const float levelL, const float levelR)
{
	string lString = "";
	string rString = "";
	int peaklValue = min((int) floor(peakL * 34), 34);
	int peakrValue = min((int) floor(peakR * 34), 34);
	int thresholdValue = (threshold + 63) * 0.53125;
	
	int levell = min((int) floor(levelL * 34), 34);
	int levelr = min((int) floor(levelR * 34), 34);

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

	findLabel("vuleft").lock()->setText(lString);
	findLabel("vuright").lock()->setText(rString);
}

int SampleScreen::getMode()
{
	return mode;
}

int SampleScreen::getMonitor()
{
	return monitor;
}

void SampleScreen::update(moduru::observer::Observable* o, nonstd::any arg)
{
	if (dynamic_cast<mpc::audiomidi::SoundRecorder*>(o) != nullptr)
	{
		try
		{
			auto vuValue = nonstd::any_cast<pair<float, float>>(arg);
			auto left = vuValue.first;
			auto right = vuValue.second;
			if (left > peakL) peakL = left;
			if (right > peakR) peakR = right;
			updateVU(left, right);
		}
		catch (const std::exception& e) {
			// nothing to do
		}
		return;
	}
}
