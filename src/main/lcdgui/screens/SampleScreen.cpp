#include <lcdgui/screens/SampleScreen.hpp>

#include <Mpc.hpp>
#include <controls/Controls.hpp>

#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/SoundRecorder.hpp>

#include <ui/sampler/SamplerGui.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::lcdgui::screens;
using namespace std;

SampleScreen::SampleScreen(const int layerIndex)
	: ScreenComponent("sample", layerIndex)
{
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
		auto lengthInFrames = samplerGui->getTime() * (44100 * 0.1);
		ams->getSoundRecorder().lock()->prepare(sound, lengthInFrames, samplerGui->getMode());
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
            auto oldInput = samplerGui->getInput();
            samplerGui->setInput(samplerGui->getInput() + i);
        }
		else if (param.compare("threshold") == 0)
		{
			samplerGui->setThreshold(samplerGui->getThreshold() + i);
		}
		else if (param.compare("mode") == 0)
		{
			samplerGui->setMode(samplerGui->getMode() + i);
		}
		else if (param.compare("time") == 0)
		{
			samplerGui->setTime(samplerGui->getTime() + i);
		}
		else if (param.compare("monitor") == 0)
		{
			samplerGui->setMonitor(samplerGui->getMonitor() + i);
			bool muteMonitor = samplerGui->getMonitor() == 0 ? true : false;
			Mpc::instance().getAudioMidiServices().lock()->muteMonitor(muteMonitor);
		}
		else if (param.compare("prerec") == 0)
		{
			samplerGui->setPreRec(samplerGui->getPreRec() + i);
		}
    //}
}
