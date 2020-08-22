#include "SampleScreen.hpp"

#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/SoundRecorder.hpp>

#include <cmath>

using namespace mpc::lcdgui::screens;
using namespace std;

SampleScreen::SampleScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "sample", layerIndex)
{
}

void SampleScreen::open()
{
	ls.lock()->getCurrentBackground()->setName("sample");

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

void SampleScreen::left()
{
	if (mpc.getAudioMidiServices().lock()->isRecordingSound())
		return;

	baseControls->left();
}

void SampleScreen::right()
{
	if (mpc.getAudioMidiServices().lock()->isRecordingSound())
		return;

	baseControls->right();
}

void SampleScreen::up()
{
	if (mpc.getAudioMidiServices().lock()->isRecordingSound())
		return;

	baseControls->up();
}

void SampleScreen::down()
{
	if (mpc.getAudioMidiServices().lock()->isRecordingSound())
		return;

	baseControls->down();
}

void SampleScreen::function(int i)
{
	init();

	switch (i)
	{
	case 0:
		if (mpc.getAudioMidiServices().lock()->isRecordingSound())
			return;

		peakL = 0.f;
		peakR = 0.f;
		break;
	case 4:
		if (mpc.getAudioMidiServices().lock()->isRecordingSound())
		{
			mpc.getAudioMidiServices().lock()->stopSoundRecorder(true);
			findBackground().lock()->setName("sample");
		}
		else if (mpc.getAudioMidiServices().lock()->getSoundRecorder().lock()->isArmed())
		{
			mpc.getAudioMidiServices().lock()->getSoundRecorder().lock()->setArmed(false);
			sampler.lock()->deleteSample(sampler.lock()->getSoundCount() - 1);
			findBackground().lock()->setName("sample");
		}
		break;
	case 5:
		if (mpc.getControls().lock()->isF6Pressed())
			return;

		mpc.getControls().lock()->setF6Pressed(true);

		auto ams = mpc.getAudioMidiServices().lock();

		if (ams->isRecordingSound())
		{
			ams->stopSoundRecorder();
			return;
		}

		if (ams->getSoundRecorder().lock()->isArmed())
		{
			ams->startRecordingSound();
			findBackground().lock()->setName("recording");
		}
		else
		{
			auto sound = sampler.lock()->addSound();
			sound.lock()->setName(sampler.lock()->addOrIncreaseNumber("sound"));
			auto lengthInFrames = time * (44100 * 0.1);
			ams->getSoundRecorder().lock()->prepare(sound, lengthInFrames);
			ams->getSoundRecorder().lock()->setArmed(true);
			findChild<Background>("").lock()->setName("waiting-for-input-signal");
		}

		break;
	}
}

void SampleScreen::turnWheel(int i)
{
    init();
	auto ams = mpc.getAudioMidiServices().lock();

	if (!ams->isRecordingSound())
	{
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
    }
}


void SampleScreen::setInput(int i)
{
	if (i < 0 || i > 1)
		return;

	input = i;
	displayInput();
}

void SampleScreen::setThreshold(int i)
{
	if (i < -64 || i > 0)
		return;

	threshold = i;
	displayThreshold();
}

void SampleScreen::setMode(int i)
{
	if (i < 0 || i > 2)
		return;

	mode = i;
	displayMode();
}

void SampleScreen::setTime(int i)
{
	if (i < 0 || i > 3786)
		return;

	time = i;
	displayTime();
}

void SampleScreen::setMonitor(int i)
{
	if (i < 0 || i > 5)
		return;

	monitor = i;
	displayMonitor();
}

void SampleScreen::setPreRec(int i)
{
	if (i < 0 || i > 100)
		return;

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

	int peaklValue = (int) floor(log10(peakL) * 20);
	int peakrValue = (int) floor(log10(peakR) * 20);
	
	int levell = (int) floor(log10(levelL) * 20);
	int levelr = (int) floor(log10(levelR) * 20);

	for (int i = 0; i < 34; i++)
	{
		string l = " ";
		string r = " ";
		bool normall = vuPosToDb[i] <= levell;
		bool normalr = vuPosToDb[i] <= levelr;
		
		bool thresholdHit = threshold >= vuPosToDb[i] && (i == 33 || threshold < vuPosToDb[i + 1]);
		
		bool peakl = peaklValue >= vuPosToDb[i] && (i == 33 || peaklValue < vuPosToDb[i + 1]);
		bool peakr = peakrValue >= vuPosToDb[i] && (i == 33 || peakrValue < vuPosToDb[i + 1]);

		if (thresholdHit && peakl) l = vu_peak_threshold;
		if (thresholdHit && peakr) r = vu_peak_threshold;

		if (thresholdHit && normall && !peakl) l = vu_normal_threshold;
		if (thresholdHit && normalr && !peakr) r = vu_normal_threshold;

		if (thresholdHit && !peakl && !normall) l = vu_threshold;
		if (thresholdHit && !peakr && !normalr) r = vu_threshold;

		if (normall && !peakl && !thresholdHit) l = vu_normal;
		if (normalr && !peakr && !thresholdHit) r = vu_normal;

		if (peakl && !thresholdHit) l = vu_peak;
		if (peakr && !thresholdHit) r = vu_peak;

		if (peakl && thresholdHit && levell == 33) l = vu_peak_threshold_normal;
		if (peakr && thresholdHit && levelr == 33) r = vu_peak_threshold_normal;

		lString += l;
		rString += r;
	}

	findLabel("vuleft").lock()->setText( (mode == 0 || mode == 2) ? lString : "                                  ");
	findLabel("vuright").lock()->setText( (mode == 1 || mode == 2) ? rString : "                                  ");
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
