#include <controls/sampler/SampleControls.hpp>

#include <Mpc.hpp>
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
	super::left();
}

void SampleControls::right() {
	//if (sampler.lock()->isArmed() || sampler.lock()->isRecording()) return;
	super::right();
}

void SampleControls::up() {
	//if (sampler.lock()->isArmed() || sampler.lock()->isRecording()) return;
	super::up();
}

void SampleControls::down() {
	//if (sampler.lock()->isArmed() || sampler.lock()->isRecording()) return;
	super::down();
}

void SampleControls::function(int i)
{
	init();
	auto lSampler = sampler.lock();
	switch (i) {
	case 0:
		//if (!lSampler->isRecording() && !lSampler->isArmed())
			//lSampler->resetPeak();
		break;
	case 4:
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
		auto sound = lSampler->addSound();
		sound.lock()->setName(lSampler->addOrIncreaseNumber("sound"));
		auto lengthInFrames = samplerGui->getTime() * 44100;
		mpc->getAudioMidiServices().lock()->getSoundRecorder().lock()->start(sound, lengthInFrames, samplerGui->getMode());
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
		}
		else if (param.compare("prerec") == 0) {
			samplerGui->setPreRec(samplerGui->getPreRec() + i);
		}
    //}
}
