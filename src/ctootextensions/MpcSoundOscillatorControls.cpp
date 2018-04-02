#include <ctootextensions/MpcSoundOscillatorControls.hpp>

using namespace mpc::ctootextensions;
using namespace std;

MpcSoundOscillatorControls::MpcSoundOscillatorControls(int id, int instanceIndex)
	: CompoundControl(id, instanceIndex, "msoc")
{
	sampleRate = 44100;
	mono = false;
	sndLevel = 100;
	tune = 0;
}

void MpcSoundOscillatorControls::setSndLevel(int i)
{
	if (i < 0 || i > 200) return;

	sndLevel = i;
	setChanged();
	notifyObservers(string("level"));
}

void MpcSoundOscillatorControls::setTune(int i)
{
	if (i < -120 || i > 120) return;

	tune = i;
	setChanged();
	notifyObservers(string("tune"));
}

void MpcSoundOscillatorControls::setLoopEnabled(bool b)
{
    loopEnabled = b;
    setChanged();
    notifyObservers(string("loopenabled"));
}

void MpcSoundOscillatorControls::setStart(int i)
{
	auto value = i;
	if (value < 0) {
		if (start == 0)
			return;
		value = 0;
	}
	else if (value >= getLastFrameIndex() + 1) {
		if (start == getLastFrameIndex() + 1)
			return;

		value = getLastFrameIndex() + 1;
	}

	start = value;

	if (start > end)
		setEnd(start);

	setChanged();
	notifyObservers(string("start"));
}

void MpcSoundOscillatorControls::setEnd(int i)
{
    auto value = i;
	if (value < 0) {
		if (end == 0) return;
		value = 0;
	}
	else if (value > getLastFrameIndex() + 1) {
		if (end == getLastFrameIndex() + 1) return;
		value = getLastFrameIndex() + 1;
	}

    end = value;
    if (end < loopTo) setLoopTo(end);
    if (end < start) setStart(end);

    setChanged();
    notifyObservers(string("end"));
}

void MpcSoundOscillatorControls::setMono(bool b)
{
	mono = b;
	end = getLastFrameIndex() + 1;
	loopTo = end;
}

void MpcSoundOscillatorControls::setLoopTo(int i)
{
	auto value = i;
	if (value < 0) {
		if (loopTo == 0) return;
		value = 0;
	}
	else if (value > getLastFrameIndex() + 1) {
		if (loopTo == getLastFrameIndex() + 1) return;
		value = getLastFrameIndex() + 1;
	}

	loopTo = value;
	if (loopTo > end) setEnd(loopTo);

	setChanged();
	notifyObservers(string("loopto"));
}


int MpcSoundOscillatorControls::getLastFrameIndex()
{
    return (isMono() ? sampleData.size() : (sampleData.size() / 2)) - 1;
}

int MpcSoundOscillatorControls::getTune()
{
    return tune;
}

bool MpcSoundOscillatorControls::isLoopEnabled()
{
    return loopEnabled;
}

int MpcSoundOscillatorControls::getStart()
{
    return start;
}

int MpcSoundOscillatorControls::getEnd()
{
    return end;
}

vector<float>* MpcSoundOscillatorControls::getSampleData()
{
    return &sampleData;
}

bool MpcSoundOscillatorControls::isMono()
{
    return mono;
}

int MpcSoundOscillatorControls::getLoopTo()
{
    return loopTo;
}

int MpcSoundOscillatorControls::getSampleRate()
{
    return sampleRate;
}

void MpcSoundOscillatorControls::setSampleRate(int sr) {
	sampleRate = sr;
}

int MpcSoundOscillatorControls::getSndLevel()
{
    return sndLevel;
}

string MpcSoundOscillatorControls::getName()
{
    return Control::getName();
}

void MpcSoundOscillatorControls::insertFrame(std::vector<float> frame, unsigned int index) {
	if (index > getLastFrameIndex() + 1) return;
	if (!mono) {
		if (frame.size() < 2) return;
		const unsigned int rightIndex = index + getLastFrameIndex();
		sampleData.insert(sampleData.begin() + rightIndex, frame[1]);
	}
	if (frame.size() < 1) return;
	sampleData.insert(sampleData.begin() + index, frame[0]);
}

MpcSoundOscillatorControls::~MpcSoundOscillatorControls() {
}
