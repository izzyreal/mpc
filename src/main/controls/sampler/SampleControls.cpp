#include <controls/sampler/SampleControls.hpp>

#include <Mpc.hpp>
#include <controls/Controls.hpp>

#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/SoundRecorder.hpp>

#include <ui/sampler/SamplerGui.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::controls::sampler;
using namespace std;

SampleControls::SampleControls(mpc::Mpc* mpc)
	: AbstractSamplerControls(mpc)
{
}

void SampleControls::left() {
	//if (sampler.lock()->isArmed() || sampler.lock()->isRecording()) return;
	if (mpc->getAudioMidiServices().lock()->isRecordingSound()) {
		return;
	}
	super::left();
}

void SampleControls::right() {
	//if (sampler.lock()->isArmed() || sampler.lock()->isRecording()) return;
	if (mpc->getAudioMidiServices().lock()->isRecordingSound()) {
		return;
	}
	super::right();
}

void SampleControls::up() {
	//if (sampler.lock()->isArmed() || sampler.lock()->isRecording()) return;
	if (mpc->getAudioMidiServices().lock()->isRecordingSound()) {
		return;
	}
	super::up();
}

void SampleControls::down() {
	//if (sampler.lock()->isArmed() || sampler.lock()->isRecording()) return;
	if (mpc->getAudioMidiServices().lock()->isRecordingSound()) {
		return;
	}
	super::down();
}

void SampleControls::function(int i)
{
	init();
	auto lSampler = sampler.lock();
	switch (i) {
	case 0:
		if (mpc->getAudioMidiServices().lock()->isRecordingSound()) {
			return;
		}
		//if (!lSampler->isRecording() && !lSampler->isArmed())
			//lSampler->resetPeak();
		break;
	case 4:
		if (mpc->getAudioMidiServices().lock()->isRecordingSound()) {
			return;
		}
		//if (lSampler->isRecording()) {
		//	lSampler->cancelRecording();
		//	return;
		//}
		//if (lSampler->isArmed()) {
		//	lSampler->unArm();
		//	return;
		//}
		break;
	case 5:
		
		if (mpc->getControls().lock()->isF6Pressed()) {
			return;
		}

		mpc->getControls().lock()->setF6Pressed(true);

		auto ams = mpc->getAudioMidiServices().lock();

		if (ams->isRecordingSound()) {
			ams->stopSoundRecorder();
			return;
		}

		auto sound = lSampler->addSound();
		sound.lock()->setName(lSampler->addOrIncreaseNumber("sound"));
		auto lengthInFrames = samplerGui->getTime() * (44100 * 0.1);
		ams->getSoundRecorder().lock()->prepare(sound, lengthInFrames, samplerGui->getMode());
		ams->startRecordingSound();

		//if (!lSampler->isRecording()) {
			//lSampler->arm();
			//return;
		//}
		//if (lSampler->isRecording()) {
			//lSampler->stopRecordingEarlier();
			//return;
		//}
		break;
	}
}

void SampleControls::turnWheel(int i)
{
    init();
	auto lSampler = sampler.lock();

	//if (!lSampler->isRecording() && !lSampler->isArmed()) {
        if (param.compare("input") == 0) {
            auto oldInput = samplerGui->getInput();
            samplerGui->setInput(samplerGui->getInput() + i);
        }
		else if (param.compare("threshold") == 0) {
			samplerGui->setThreshold(samplerGui->getThreshold() + i);
		}
		else if (param.compare("mode") == 0) {
			samplerGui->setMode(samplerGui->getMode() + i);
		}
		else if (param.compare("time") == 0) {
			samplerGui->setTime(samplerGui->getTime() + i);
		}
		else if (param.compare("monitor") == 0) {
			samplerGui->setMonitor(samplerGui->getMonitor() + i);
			bool muteMonitor = samplerGui->getMonitor() == 0 ? true : false;
			mpc->getAudioMidiServices().lock()->muteMonitor(muteMonitor);
		}
		else if (param.compare("prerec") == 0) {
			samplerGui->setPreRec(samplerGui->getPreRec() + i);
		}
    //}
}
