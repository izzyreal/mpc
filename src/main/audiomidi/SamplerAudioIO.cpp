#include "SamplerAudioIO.hpp"

using namespace mpc::audiomidi;

using namespace ctoot::audio::server;
using namespace ctoot::audio::system;

using namespace std;

SamplerAudioIO::SamplerAudioIO(mpc::Mpc* mpc) {
	monitorOutput = make_shared<MonitorOutput>(mpc);
}

void SamplerAudioIO::changeSampleRate(int newRate) {
//	recordBuffer = make_unique<ctoot::audio::core::AudioBuffer>("record", 2, 512, rate);
//	preRecBuffer.push_back(circular_buffer<float>(rate * 0.1));
//	preRecBuffer.push_back(circular_buffer<float>(rate * 0.1));
}

void SamplerAudioIO::work(int nFrames)
{
	/*
	if (input == nullptr && inputSwap == nullptr) return;
	auto ls = mpc->getLayeredScreen().lock();
	if (!ls) return;
	if (ls->getCurrentScreenName().compare("sample") != 0) return;

	auto arm = false;
	if (inputSwap != nullptr) {
		input = inputSwap;
		inputSwap = nullptr;
	}

	if (recordBuffer->getSampleCount() != nFrames) {
		recordBuffer->changeSampleCount(nFrames, false);
	}

	input->processAudio(recordBuffer.get());

	monitorBufferL = recordBuffer->getChannel(0);
	monitorBufferR = recordBuffer->getChannel(1);

	for (int i = 0; i < nFrames; i++) {

		monitorBufferL->at(i) *= inputLevel * 0.01;
		monitorBufferR->at(i) *= inputLevel * 0.01;

		if (armed && abs(monitorBufferL->at(i)) > (mpc->getUis().lock()->getSamplerGui()->getThreshold() + 64) * 0.015625) {
			arm = true;
		}

		if (recording) {
			recordBufferL[recordFrame] = (*monitorBufferL)[i];
			recordBufferR[recordFrame++] = (*monitorBufferR)[i];
			if (recordFrame == recordBufferL.size()) {
				stopRecording();
			}
		}
		else {
			//            preRecBufferL.push_back((*monitorBufferL)[i]);
			//            preRecBufferR.push_back((*monitorBufferR)[i]);
		}

		if (monitorBufferL->at(i) > 0 && vuCounter++ == 5) {

			vuCounter = 0;

			if (monitorBufferL->at(i) > peakL) {
				peakL = monitorBufferL->at(i);
			}

			if (monitorBufferR->at(i) > peakR) {
				peakR = monitorBufferR->at(i);
			}

			vuBufferL[vuSampleCounter] = (*monitorBufferL)[i] < 0.01 ? 0 : (*monitorBufferL)[i];
			vuBufferR[vuSampleCounter] = (*monitorBufferR)[i] < 0.01 ? 0 : (*monitorBufferR)[i];
			if (vuSampleCounter == VU_BUFFER_SIZE) {
				vuSampleCounter = 0;
				float highestLeft = 0;

				for (auto& sample : vuBufferL) {
					if (sample > highestLeft) {
						highestLeft = sample;
					}
				}

				float highestRight = 0;
				for (auto& sample : vuBufferR) {
					if (sample > highestRight) {
						highestRight = sample;
					}
				}

				levelL = highestLeft;
				levelR = highestRight;

				mpc->getUis().lock()->getSamplerGui()->notify("vumeter");
				vuBufferL.clear();
				vuBufferR.clear();
			}
		}
	}

	if (arm) {
		arm = false;
		armed = false;
		record();
	}
	*/
}

void SamplerAudioIO::setEnabled(bool enabled)
{
	this->enabled = enabled;
}

string SamplerAudioIO::getName()
{
	return "sampler";
}

vector<weak_ptr<AudioInput>> SamplerAudioIO::getAudioInputs()
{
	return {};
}

vector<weak_ptr<ctoot::audio::system::AudioOutput>> SamplerAudioIO::getAudioOutputs()
{
	return vector<weak_ptr<AudioOutput>>{ monitorOutput };
}

void SamplerAudioIO::closeAudio()
{
	monitorOutput->close();
}

SamplerAudioIO::~SamplerAudioIO() {
}

