#include <ui/vmpc/AudioGui.hpp>
#include <audiomidi/AudioMidiServices.hpp>

using namespace mpc::ui::vmpc;
using namespace std;

AudioGui::AudioGui(weak_ptr<mpc::audiomidi::AudioMidiServices> ams)
{
	this->ams = ams;
	inputDevs = vector<int>(2);
	outputDevs = vector<int>(5);
}

void AudioGui::setInputDevs(vector<int> devs) {
	inputDevs = devs;
}

void AudioGui::setOutputDevs(vector<int> devs) {
	outputDevs = devs;
}

void AudioGui::setDev0(int i) {
	int max = -1;
	int increment = inputDevs[in] > i ? -1 : 1;
	auto lAms = ams.lock();
/*
	if (lAms->getCurrentServerName().compare("DirectSound") == 0) {
		max = lAms->getDirectSoundInputDevCount();
	}
	else {
		max = lAms->getInputNames().size();
	}

	for (int& inputIndex : inputDevs) {
		if (inputIndex == i && i != -1) {
			i += increment;
			break;
		}
	}

	if (i < -1 || i >= max) return;
	inputDevs[in] = i;
	*/
	setChanged();
	notifyObservers(string("dev0"));
}

void AudioGui::setDev1(int i) {
	int max = -1;
	int increment = outputDevs[out] > i ? -1 : 1;
	auto lAms = ams.lock();
	/*
	if (lAms->getCurrentServerName().compare("DirectSound") == 0) {
		max = lAms->getDirectSoundOutputDevCount();
	}
	else {
		max = lAms->getOutputNames().size();
	}

	bool exists = true;
	while (exists && i != -1) {
		exists = false;
		for (int& outputIndex : outputDevs) {
			if (outputIndex == i) {
				i += increment;
				exists = true;
				break;
			}
		}
	}

	if (i < -1 || i >= max) return;
	outputDevs[out] = i;
	*/
	setChanged();
	notifyObservers(string("dev1"));
}

int AudioGui::getDev0() {
	return inputDevs[in];
}

int AudioGui::getDev1() {
	return outputDevs[out];
}

void AudioGui::setIn(int i)
{
	if (i < 0 || i > 1) return;
	in = i;
	setChanged();
	notifyObservers(string("in"));
}

vector<int> AudioGui::getInputDevs() {
	return inputDevs;
}

vector<int> AudioGui::getOutputDevs() {
	return outputDevs;
}

int AudioGui::getIn()
{
	return in;
}

void AudioGui::setOut(int i)
{
	if (i < 0 || i > 4) return;
	out = i;
	setChanged();
	notifyObservers(string("out"));
}

int AudioGui::getOut()
{
	return out;
}


void AudioGui::setServer(int i) {
	//if (i < -1 || i >= ams.lock()->getServerCount()) return;
	server = i;
	setChanged();
	notifyObservers(string("server"));
}

int AudioGui::getServer() {
	return server;
}
