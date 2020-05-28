#include "SampleScreen.hpp"

#include <Mpc.hpp>
#include <controls/Controls.hpp>

#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/SoundRecorder.hpp>

#include <sampler/Sampler.hpp>

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

	auto ams = Mpc::instance().getAudioMidiServices().lock();
	ams->getSoundRecorder().lock()->addObserver(this);
}

void SampleScreen::close()
{
	auto ams = Mpc::instance().getAudioMidiServices().lock();
	ams->getSoundRecorder().lock()->deleteObserver(this);

}

void SampleScreen::left() {
	//if (sampler.lock()->isArmed() || sampler.lock()->isRecording()) return;
	if (Mpc::instance().getAudioMidiServices().lock()->isRecordingSound())
	{
		return;
	}
	BaseControls::left();
}

void SampleScreen::right() {
	//if (sampler.lock()->isArmed() || sampler.lock()->isRecording()) return;
	if (Mpc::instance().getAudioMidiServices().lock()->isRecordingSound()) {
		return;
	}
	BaseControls::right();
}

void SampleScreen::up() {
	//if (sampler.lock()->isArmed() || sampler.lock()->isRecording()) return;
	if (Mpc::instance().getAudioMidiServices().lock()->isRecordingSound()) {
		return;
	}
	BaseControls::up();
}

void SampleScreen::down() {
	//if (sampler.lock()->isArmed() || sampler.lock()->isRecording()) return;
	if (Mpc::instance().getAudioMidiServices().lock()->isRecordingSound()) {
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
		if (Mpc::instance().getAudioMidiServices().lock()->isRecordingSound())
		{
			return;
		}
		//if (!samplerisRecording() && !samplerisArmed())
			//samplerresetPeak();
		break;
	case 4:
		if (Mpc::instance().getAudioMidiServices().lock()->isRecordingSound())
		{
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
		if (Mpc::instance().getControls().lock()->isF6Pressed())
		{
			return;
		}

		Mpc::instance().getControls().lock()->setF6Pressed(true);

		auto ams = Mpc::instance().getAudioMidiServices().lock();

		if (ams->isRecordingSound())
		{
			ams->stopSoundRecorder();
			return;
		}

		auto sound = sampler.lock()->addSound();
		sound.lock()->setName(sampler.lock()->addOrIncreaseNumber("sound"));
		auto lengthInFrames = time * (44100 * 0.1);
		ams->getSoundRecorder().lock()->prepare(sound, lengthInFrames, mode);
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
			Mpc::instance().getAudioMidiServices().lock()->muteMonitor(muteMonitor);
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
	auto thresholdText = threshold == -64 ? "-\u00D9\u00DA" : to_string(threshold);
	findField("threshold").lock()->setText(thresholdText);
}

void SampleScreen::displayMode()
{
	findField("mode").lock()->setText(modeNames[mode]);
}

void SampleScreen::displayTime()
{
	string timeText = to_string(time);
	timeText = timeText.substr(0, timeText.length() - 1) + "." + timeText.substr(timeText.length() - 1);
	findField("time").lock()->setText(timeText);
}

void SampleScreen::displayMonitor()
{
	findField("monitor").lock()->setText(monitorNames[monitor]);
}

void SampleScreen::displayPreRec()
{
	findField("prerec").lock()->setText(to_string(preRec) + "ms");
}

void SampleScreen::updateVU(float value)
{
	string lString = "";
	string rString = "";
	auto peaklValue = value;
	auto peakrValue = value;
	int thresholdValue = (threshold + 63) * 0.53125;
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

	findLabel("vuleft").lock()->setText(lString);
	findLabel("vuright").lock()->setText(lString);
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
			auto vuValue = nonstd::any_cast<float>(arg);
			updateVU(vuValue * 100);
		}
		catch (const std::exception& e) {
			// nothing to do
		}
		return;
	}
}
