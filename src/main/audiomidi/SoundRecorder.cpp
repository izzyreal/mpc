#include <audiomidi/SoundRecorder.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/SampleScreen.hpp>

#include <audio/core/AudioFormat.hpp>
#include <audio/core/AudioBuffer.hpp>

#include <cmath>

using namespace mpc::sampler;
using namespace mpc::audiomidi;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace std;

unsigned int SoundRecorder::getInputGain()
{
	return inputGain;
}

void SoundRecorder::setInputGain(unsigned int gain)
{
	if (gain < 0 || gain > 100) {
		return;
	}
	inputGain = gain;
}

// modes: 0 = MONO L, 1 = MONO R, 2 = STEREO
void SoundRecorder::prepare(const weak_ptr<Sound> sound, int lengthInFrames)
{
	if (recording) {
		return;
	}

	this->sound = sound;
	this->lengthInFrames = lengthInFrames;

	if (mode != 2) {
		sound.lock()->setMono(true);
	}
}

// Should be called from the audio thread
void SoundRecorder::start() {

	if (recording) {
		return;
	}

	resampleBufferLeft.reset();
	resampleBufferRight.reset();
	recording = true;
}

bool SoundRecorder::isRecording() {
	return recording;
}

void SoundRecorder::stop() {
	recording = false;
	auto s = sound.lock();

	auto frameCount = s->getOscillatorControls()->getFrameCount();
	auto overflow = frameCount - lengthInFrames;

	if (overflow > 0) {
		s->getSampleData()->erase(s->getSampleData()->end() - overflow, s->getSampleData()->end());
		if (mode == 2) {
			s->getSampleData()->erase(s->getSampleData()->begin() + lengthInFrames , s->getSampleData()->begin() + frameCount);
		}
	}

	s->setEnd(s->getOscillatorControls()->getFrameCount());

	if (srcLeft != NULL) {
		src_delete(srcLeft);
		srcLeft = NULL;
	}

	if (srcRight != NULL) {
		src_delete(srcRight);
		srcRight = NULL;
	}
}

void applyGain(float gain, vector<float>* data)
{
	for (int i = 0; i < data->size(); i++) {
		(*data)[i] *= gain;
	}
}

void SoundRecorder::setSampleScreenActive(bool active)
{
	sampleScreenActive.store(active);
}

int SoundRecorder::processAudio(ctoot::audio::core::AudioBuffer* buf)
{
	auto left = buf->getChannel(0);
	auto right = buf->getChannel(1);

	applyGain(inputGain * 0.01, left);
	applyGain(inputGain * 0.01, right);

	if (sampleScreenActive.load()) {

		if (!lastSampleScreenActive) {
			lastSampleScreenActive = true;
		}

		auto sampleScreen = dynamic_pointer_cast<SampleScreen>(Screens::getScreenComponent("sample"));
		mode = sampleScreen->getMode();

		float peakL = 0, peakR = 0;
		
		for (auto& f : (*left))
		{
			if ((mode == 0 || mode == 2) && abs(f) > peakL) peakL = abs(f);
			if (!recording) preRecBufferLeft.put(f);
		}

		for (auto& f : (*right))
		{
			if ((mode == 1 || mode == 2) && abs(f) > peakR) peakR = abs(f);
			if (!recording) preRecBufferRight.put(f);
		}

		notifyObservers(pair<float, float>(peakL, peakR));
	}
	else {
		if (lastSampleScreenActive)
		{
			preRecBufferLeft.reset();
			preRecBufferLeft.reset();
			lastSampleScreenActive = false;
		}
	}

	if (recording) {
		auto s = sound.lock();
		auto osc = s->getOscillatorControls();
		auto currentLength = s->getOscillatorControls()->getFrameCount();
		auto resample = buf->getSampleRate() != 44100;

		if (resample) {
			if (((mode == 0 || mode == 2) && srcLeft == NULL) ||
				((mode == 1 || mode == 2) && srcRight == NULL)) {
				initSrc();
			}
		}

		vector<float> resampledLeft;

		if ((mode == 0 || mode == 2) && resample) {
			resampledLeft = resampleChannel(true, left, buf->getSampleRate());
			left = &resampledLeft;
		}

		vector<float> resampledRight;

		if ((mode == 1 || mode == 2) && resample) {
			resampledRight = resampleChannel(false, right, buf->getSampleRate());
			right = &resampledRight;
		}

		if (mode == 0) {
			osc->insertFrames(*left, currentLength);
		}
		else if (mode == 1) {
			osc->insertFrames(*right, currentLength);
		}
		else if (mode == 2) {
			osc->insertFrames(*left, *right, currentLength);
		}

		if (osc->getFrameCount() >= lengthInFrames) {
			recording = false;
		}

		return AUDIO_SILENCE;
	}

	return AUDIO_SILENCE;
}

vector<float> SoundRecorder::resampleChannel(bool left, vector<float>* buffer, int sourceSampleRate)
{
	auto ratio = 44100.f / sourceSampleRate;
	auto circBuf = left ? &resampleBufferLeft : &resampleBufferRight;

	for (auto f : (*buffer)) {
		circBuf->put(f);
	}

	vector<float> input;
	while (!circBuf->empty()) {
		input.push_back(circBuf->get());
	}

	vector<float> res(ceil(input.size() * ratio));

	SRC_DATA data;
	data.data_in = &input[0];
	data.input_frames = input.size();
	data.data_out = &res[0];
	data.output_frames = res.size();
	data.end_of_input = 0;
	data.src_ratio = ratio;

	src_process(left ? srcLeft : srcRight, &data);

	circBuf->move(-(input.size() - data.input_frames_used));
	res.resize(data.output_frames_gen);
	return res;
}

void SoundRecorder::initSrc() {
	if (mode == 0 || mode == 2) {
		srcLeft = src_new(0, 1, &srcLeftError);
	}

	if (mode == 1 || mode == 2) {
		srcRight = src_new(0, 1, &srcRightError);
	}
}
