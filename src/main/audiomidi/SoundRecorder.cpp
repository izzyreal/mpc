#include <audiomidi/SoundRecorder.hpp>

#include <audio/core/AudioFormat.hpp>
#include <audio/core/AudioBuffer.hpp>

#include <chrono>
#include <thread>

using namespace std;
using namespace mpc::sampler;
using namespace mpc::audiomidi;

SoundRecorder::SoundRecorder()
{
}


// modes: 0 = MONO L, 1 = MONO R, 2 = STEREO
void SoundRecorder::start(const weak_ptr<Sound> sound, int lengthInFrames, int mode)
{
	this->sound = sound;
	this->lengthInFrames = lengthInFrames;
	this->mode = mode;
	
	if (mode != 2) {
		sound.lock()->setMono(true);
	}
	
	recordedFrameCount = 0;
	recording = true;
}

int SoundRecorder::processAudio(ctoot::audio::core::AudioBuffer* buf)
{
	if (recording) {	
		auto s = sound.lock();
		auto currentLength = s->getLastFrameIndex() + 1;

		auto left = buf->getChannel(0);
		auto right = buf->getChannel(1);

		auto prospectiveLength = s->getLastFrameIndex() + 1 + buf->getSampleCount();

		if (prospectiveLength < lengthInFrames) {
			
			if (mode == 1 || mode == 2) {
				s->getOscillatorControls()->insertFrames(*right, s->getSampleData()->size());
			}
			
			if (mode == 0 || mode == 2) {
				s->getOscillatorControls()->insertFrames(*left, currentLength);
			}
		}
		else {
			auto overflow = prospectiveLength - lengthInFrames;
			auto trimmedLeft = vector<float>(left->begin(), left->end() - overflow);
			auto trimmedRight = vector<float>(right->begin(), right->end() - overflow);
		
			if (mode == 1 || mode == 2) {
				s->getOscillatorControls()->insertFrames(trimmedRight, s->getSampleData()->size());
			}

			if (mode == 0 || mode == 2) {
				s->getOscillatorControls()->insertFrames(trimmedLeft, currentLength);
			}

			recording = false;
		}
		return AUDIO_SILENCE;
	}

	return AUDIO_SILENCE;
}

SoundRecorder::~SoundRecorder() {
}
