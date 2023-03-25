#include "ExternalAudioServer.hpp"
#include "StereoInputProcess.hpp"
#include "StereoOutputProcess.hpp"
#include "../core/ChannelFormat.hpp"

using namespace mpc::engine::audio::server;
using namespace std;

void ExternalAudioServer::start()
{
	if (running)
		return;
	
	running = true;
}

void ExternalAudioServer::stop()
{
	running = false;
}

bool ExternalAudioServer::isRunning()
{
	return running;
}

void ExternalAudioServer::close()
{
	for (auto& i : activeInputs)
		if (i != nullptr) delete i;

	for (auto& o : activeOutputs)
		if (o != nullptr) delete o;

	activeInputs.clear();
	activeOutputs.clear();
}

void ExternalAudioServer::resizeBuffers(int newSize)
{
	for (auto& i : activeInputs)
	{
		if (i->localBuffer.size() != newSize * 2)
			i->localBuffer.resize(newSize * 2);
	}

	for (auto& o : activeOutputs)
	{
		if (o->localBuffer.size() != newSize* 2)
			o->localBuffer.resize(newSize * 2);
	}
	
	AudioServer::resizeBuffers(newSize);
}

void ExternalAudioServer::work(float* inputBuffer, float* outputBuffer, int nFrames, int inputChannelCount, int outputChannelCount) {
	if (!running) {
		return;
	}
	
	int sampleCounter = 0;
	const int inputsToProcess = min((int)(inputChannelCount * 0.5), (int)activeInputs.size());

	for (int frame = 0; frame < nFrames; frame++)
	{
		for (int input = 0; input < inputsToProcess; input++)
		{
			activeInputs[input]->localBuffer[sampleCounter++] = *inputBuffer++;
			activeInputs[input]->localBuffer[sampleCounter++] = *inputBuffer++;
		}
	}

	client->work(nFrames);

	const int outputsToProcess = outputChannelCount * 0.5;

	for (int frame = 0; frame < nFrames; frame++)
	{
		for (int output = 0; output < outputsToProcess; output++)
		{
			if (output >= activeOutputs.size())
			{
				*outputBuffer++ = 0.0f;
				*outputBuffer++ = 0.0f;
				continue;
			}
			
			*outputBuffer++ = activeOutputs[output]->localBuffer[(frame * 2)];
			*outputBuffer++ = activeOutputs[output]->localBuffer[(frame * 2) + 1];
		}
	}
}

void ExternalAudioServer::work(const float** inputBuffer, float** outputBuffer, int nFrames, int inputChannelCount, int outputChannelCount) {
	if (!running) {
		return;
	}
	
	int sampleCounter = 0;
	const int inputsToProcess = min((int) (inputChannelCount * 0.5), (int)activeInputs.size());

	for (int frame = 0; frame < nFrames; frame++)
	{
		int channelCounter = 0;
		
		for (int input = 0; input < inputsToProcess; input ++)
		{
			activeInputs[input]->localBuffer[sampleCounter++] = inputBuffer[channelCounter][frame];
			activeInputs[input]->localBuffer[sampleCounter++] = inputBuffer[channelCounter + 1][frame];
			channelCounter += 2;
		}
	}

	client->work(nFrames);

	const int outputsToProcess = outputChannelCount * 0.5;

	for (int frame = 0; frame < nFrames; frame++)
	{
		int channelCounter = 0;
	
		for (int output = 0; output < outputsToProcess; output++)
		{
			if (output >= activeOutputs.size())
			{
				outputBuffer[channelCounter][frame] = 0.0f;
				outputBuffer[channelCounter + 1][frame] = 0.0f;
				channelCounter += 2;
				continue;
			}
			
			const auto frame_x2 = frame * 2;
			outputBuffer[channelCounter][frame] = activeOutputs[output]->localBuffer[frame_x2];
			outputBuffer[channelCounter + 1][frame] = activeOutputs[output]->localBuffer[frame_x2 + 1];
			channelCounter += 2;
		}
	}
}

void ExternalAudioServer::work(const float* const* inputBuffer, float* const* outputBuffer, int nFrames, int inputChannelCount, int outputChannelCount) {
	if (!running) {
		return;
	}

	int sampleCounter = 0;
	const int inputsToProcess = min((int) (inputChannelCount * 0.5), (int)activeInputs.size());

	for (int frame = 0; frame < nFrames; frame++)
	{
		int channelCounter = 0;

		for (int input = 0; input < inputsToProcess; input ++)
		{
			activeInputs[input]->localBuffer[sampleCounter++] = inputBuffer[channelCounter][frame];
			activeInputs[input]->localBuffer[sampleCounter++] = inputBuffer[channelCounter + 1][frame];
			channelCounter += 2;
		}
	}

	client->work(nFrames);

	const int outputsToProcess = outputChannelCount * 0.5;

	for (int frame = 0; frame < nFrames; frame++)
	{
		int channelCounter = 0;

		for (int output = 0; output < outputsToProcess; output++)
		{
			if (output >= activeOutputs.size())
			{
				outputBuffer[channelCounter][frame] = 0.0f;
				outputBuffer[channelCounter + 1][frame] = 0.0f;
				channelCounter += 2;
				continue;
			}

			const auto frame_x2 = frame * 2;
			outputBuffer[channelCounter][frame] = activeOutputs[output]->localBuffer[frame_x2];
			outputBuffer[channelCounter + 1][frame] = activeOutputs[output]->localBuffer[frame_x2 + 1];
			channelCounter += 2;
		}
	}
}

void ExternalAudioServer::setClient(shared_ptr<AudioClient> client)
{
	auto lClient = client;
	this->client = lClient.get();
}

vector<string> ExternalAudioServer::getAvailableOutputNames()
{
	vector<string> res{ "STEREO OUT", "ASSIGNABLE MIX OUT 1/2", "ASSIGNABLE MIX OUT 3/4", "ASSIGNABLE MIX OUT 5/6", "ASSIGNABLE MIX OUT 7/8" };
	return res;
}

vector<string> ExternalAudioServer::getAvailableInputNames()
{
	vector<string> res{ "RECORD IN" };
	return res;
}

IOAudioProcess* ExternalAudioServer::openAudioOutput(string name)
{
	activeOutputs.push_back(new StereoOutputProcess(name));
	return activeOutputs.back();
}

IOAudioProcess* ExternalAudioServer::openAudioInput(string name)
{
	activeInputs.push_back(new StereoInputProcess(name));
	return activeInputs.back();
}

void ExternalAudioServer::closeAudioOutput(mpc::engine::audio::server::IOAudioProcess* output)
{
	if (output == nullptr)
		return;

	output->close();

	for (int i = 0; i < activeOutputs.size(); i++)
	{
		if (activeOutputs[i] == output)
		{
			activeOutputs.erase(activeOutputs.begin() + i);
			break;
		}
	}
}

void ExternalAudioServer::closeAudioInput(mpc::engine::audio::server::IOAudioProcess* input)
{
	if (input == nullptr)
		return;
	
	input->close();

	for (int i = 0; i < activeInputs.size(); i++)
	{
		if (activeInputs[i] == input)
		{
			activeInputs.erase(activeInputs.begin() + i);
			break;
		}
	}
}

int ExternalAudioServer::getInputLatencyFrames()
{
	return bufferSize;
}

int ExternalAudioServer::getOutputLatencyFrames()
{
	return bufferSize;
}

int ExternalAudioServer::getTotalLatencyFrames()
{
	return getInputLatencyFrames() + getOutputLatencyFrames();
}
