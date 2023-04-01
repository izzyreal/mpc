#include <audiomidi/SoundRecorder.hpp>

#include <audiomidi/AudioMidiServices.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/SampleScreen.hpp>

#include <cmath>

using namespace mpc::sampler;
using namespace mpc::audiomidi;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

SoundRecorder::SoundRecorder(mpc::Mpc& mpc)
	: mpc(mpc)
{
}

unsigned int SoundRecorder::getInputGain()
{
	return inputGain;
}

void SoundRecorder::setInputGain(unsigned int gain)
{
	if (gain > 100)
		return;

	inputGain = gain;
}

void SoundRecorder::setArmed(bool b)
{
	armed = b;
}

bool SoundRecorder::isArmed()
{
	return armed;
}

// modes: 0 = MONO L, 1 = MONO R, 2 = STEREO
void SoundRecorder::prepare(const std::weak_ptr<Sound> newSound, int newLengthInFrames)
{
	if (recording)
		return;

	cancelled = false;

	this->sound = newSound;
	this->lengthInFrames = newLengthInFrames;

	if (mode != 2)
		newSound.lock()->setMono(true);
}

// Should be called from the audio thread
void SoundRecorder::start()
{
	if (recording)
		return;

	mpc.getLayeredScreen()->getCurrentBackground()->setName("recording");

	armed = false;

	resampleBufferLeft.reset();
	resampleBufferRight.reset();
	recording = true;
}

bool SoundRecorder::isRecording()
{
	return recording;
}

void SoundRecorder::stop()
{
	recording = false;

	if (srcLeft != nullptr)
	{
		src_delete(srcLeft);
		srcLeft = nullptr;
	}

	if (srcRight != nullptr)
	{
		src_delete(srcRight);
		srcRight = nullptr;
	}

	if (cancelled)
	{
		mpc.getSampler()->deleteSound(sound);
		cancelled = false;
	}
	else
	{
		auto s = sound.lock();

		auto sampleScreen = mpc.screens->get<SampleScreen>("sample");
		auto preRecFrames = (int)(44.1 * sampleScreen->preRec);
		auto frameCount = s->getFrameCount();

		auto overflow = frameCount - lengthInFrames - preRecFrames; // Would be fun to check if overflow is ever not 0.

		if (overflow > 0)
		{
			s->getSampleData()->erase(s->getSampleData()->end() - overflow, s->getSampleData()->end());

			if (mode == 2)
				s->getSampleData()->erase(s->getSampleData()->begin() + lengthInFrames + preRecFrames, s->getSampleData()->end());
		}

        s->setStart(preRecFrames);
		s->setEnd(s->getFrameCount());

		mpc.getLayeredScreen()->openScreen("keep-or-retry");
	}
}

void SoundRecorder::cancel()
{
	cancelled = true;
}

void applyGain(float gain, std::vector<float>& data)
{
	for (int i = 0; i < data.size(); i++)
		data[i] *= gain;
}

void SoundRecorder::setSampleScreenActive(bool active)
{
	sampleScreenActive.store(active);
}

int SoundRecorder::processAudio(mpc::engine::audio::core::AudioBuffer* buf, int nFrames)
{
	auto left = buf->getChannel(0);
	auto right = buf->getChannel(1);

	applyGain(inputGain * 0.01, left);
	applyGain(inputGain * 0.01, right);

	auto sampleScreen = mpc.screens->get<SampleScreen>("sample");

	if (sampleScreenActive.load())
	{
		if (!lastSampleScreenActive)
			lastSampleScreenActive = true;

		mode = sampleScreen->getMode();

		float peakL = 0, peakR = 0;
		
		int frameCounter = 0;

		for (auto& f : left)
		{
			if ((mode == 0 || mode == 2) && abs(f) > peakL) peakL = abs(f);
			if (!recording) preRecBufferLeft.put(f);
			frameCounter++;
			if (frameCounter >= nFrames) break;
		}

		frameCounter = 0;
		for (auto& f : right)
		{
			if ((mode == 1 || mode == 2) && abs(f) > peakR) peakR = abs(f);
			if (!recording) preRecBufferRight.put(f);
			frameCounter++;
			if (frameCounter >= nFrames) break;
		}

		// Is this comparison correct or does the real 2KXL take Mode into account?
		// Also, does the real 2KXL do the below in a frame-accurate manner?
		if (armed && (log10(peakL) * 20 > sampleScreen->threshold || log10(peakR) * 20 > sampleScreen->threshold))
		{
			armed = false;
			mpc.getLayeredScreen()->getCurrentBackground()->setName("recording");
			mpc.getAudioMidiServices()->startRecordingSound();
		}

		notifyObservers(std::pair<float, float>(peakL, peakR));
	}
	else
	{
		if (lastSampleScreenActive)
		{
			preRecBufferLeft.reset();
			preRecBufferLeft.reset();
			lastSampleScreenActive = false;
		}
	}

	if (recording)
	{
		auto s = sound.lock();
		auto currentLength = s->getFrameCount();
		auto resample = buf->getSampleRate() != 44100;

		if (resample)
		{
			if (((mode == 0 || mode == 2) && srcLeft == nullptr) ||
				((mode == 1 || mode == 2) && srcRight == nullptr))
			{
				initSrc();
			}
		}

		if (currentLength == 0 && sampleScreen->preRec != 0)
		{
			auto preRecFrames = (int)(buf->getSampleRate() * 0.001 * sampleScreen->preRec);
			std::vector<float> preLeft(preRecFrames);
			std::vector<float> preRight(preRecFrames);

			int offset = 0;
			
			if (preRecFrames > preRecBufferLeft.available())
				offset = preRecFrames - preRecBufferLeft.available();

			for (int i = 0; i < preRecFrames; i++)
			{
				if (i < offset)
				{
					preLeft[i] = 0;
					preRight[i] = 0;
					continue;
				}

				preLeft[i] = preRecBufferLeft.empty() ? 0 : preRecBufferLeft.get();
				preRight[i] = preRecBufferRight.empty() ? 0 : preRecBufferRight.get();
			}

			if (resample)
			{
				auto resampledLeft = resampleChannel(true, preLeft, buf->getSampleRate(), nFrames);
				auto resampledRight = resampleChannel(false, preRight, buf->getSampleRate(), nFrames);

				if (mode == 0) {
					s->insertFrames(resampledLeft, 0);
				}
				else if (mode == 1) {
					s->insertFrames(resampledRight, 0);
				}
				else if (mode == 2) {
					s->insertFrames(resampledLeft, resampledRight, 0);
				}
			}
			else {
				if (mode == 0) {
					s->insertFrames(preLeft, 0);
				}
				else if (mode == 1) {
					s->insertFrames(preRight, 0);
				}
				else if (mode == 2) {
					s->insertFrames(preLeft, preRight, 0);
				}
			}
			
			preRecBufferLeft.reset();
			preRecBufferRight.reset();
		}

		std::vector<float> resampledLeft;

		if ((mode == 0 || mode == 2) && resample) {
			resampledLeft = resampleChannel(true, left, buf->getSampleRate(), nFrames);
			left = resampledLeft;
		}

		std::vector<float> resampledRight;

		if ((mode == 1 || mode == 2) && resample) {
			resampledRight = resampleChannel(false, right, buf->getSampleRate(), nFrames);
			right = resampledRight;
		}

		if (mode == 0)
			s->insertFrames(left, currentLength);
		else if (mode == 1)
			s->insertFrames(right, currentLength);
		else if (mode == 2)
			s->insertFrames(left, right, currentLength);

		if (s->getFrameCount() >= lengthInFrames) {
			recording = false;
		}

		return AUDIO_SILENCE;
	}

	return AUDIO_SILENCE;
}

std::vector<float> SoundRecorder::resampleChannel(bool left, std::vector<float>& buffer, int sourceSampleRate, int numFrames)
{
	auto ratio = 44100.f / sourceSampleRate;
	auto circBuf = left ? &resampleBufferLeft : &resampleBufferRight;

    for (int i = 0; i < numFrames; i++) {
		circBuf->put(buffer[i]);
	}

	std::vector<float> input;
	while (!circBuf->empty()) {
		input.push_back(circBuf->get());
	}

	std::vector<float> res(ceil(input.size() * ratio));

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
        if (srcLeft != nullptr)
            src_delete(srcLeft);
		srcLeft = src_new(0, 1, &srcLeftError);
	}

	if (mode == 1 || mode == 2) {
        if (srcRight != nullptr)
            src_delete(srcRight);
		srcRight = src_new(0, 1, &srcRightError);
	}
}

SoundRecorder::~SoundRecorder()
{
    if (srcLeft != nullptr)
        src_delete(srcLeft);
    if (srcRight != nullptr)
        src_delete(srcRight);
}
