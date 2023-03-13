#include "MpcSoundOscillatorControls.hpp"

using namespace ctoot::mpc;
using namespace std;

MpcSoundOscillatorControls::MpcSoundOscillatorControls(int id)
	: CompoundControl(id, "msoc")
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
}

void MpcSoundOscillatorControls::setTune(int i)
{
	if (i < -120 || i > 120) return;

	tune = i;
}

void MpcSoundOscillatorControls::setLoopEnabled(bool b)
{
    loopEnabled = b;
}

void MpcSoundOscillatorControls::setStart(int i)
{
	auto value = i;
	if (value < 0) {
		if (start == 0)
			return;
		value = 0;
	}
	else if (value >= getFrameCount()) {
		if (start == getFrameCount())
			return;

		value = getFrameCount();
	}

	start = value;

	if (start > end)
		setEnd(start);
}

void MpcSoundOscillatorControls::setEnd(int i)
{
    auto value = i;

	if (value < 0) {
		
		if (end == 0)
			return;

		value = 0;
	}
	else if (value > getFrameCount())
	{
		if (end == getFrameCount())
			return;
	
		value = getFrameCount();
	}

    end = value;

	if (end < loopTo)
		setLoopTo(end);
    
	if (end < start)
		setStart(end);
}

void MpcSoundOscillatorControls::setMono(bool b)
{
	mono = b;
	end = getFrameCount();
	loopTo = end;
}

void MpcSoundOscillatorControls::setLoopTo(int i)
{
	auto value = i;
	
	if (value < 0)
		value = 0;
	else if (value > getFrameCount())
		value = getFrameCount();

	loopTo = value;
}


int MpcSoundOscillatorControls::getLastFrameIndex()
{
    return (isMono() ? sampleData.size() : (sampleData.size() * 0.5)) - 1;
}

int MpcSoundOscillatorControls::getFrameCount()
{
	return getLastFrameIndex() + 1;
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

void MpcSoundOscillatorControls::setSampleRate(int sr)
{
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

void MpcSoundOscillatorControls::insertFrame(std::vector<float> frame, unsigned int index)
{
	if (index > getFrameCount()) {
		return;
	}
	
	if (!mono) {
		if (frame.size() < 2) return;
		const unsigned int rightIndex = index + getFrameCount();
		sampleData.insert(sampleData.begin() + rightIndex, frame[1]);
	}
	
	if (frame.size() < 1) {
		return;
	}
	
	sampleData.insert(sampleData.begin() + index, frame[0]);
}

void MpcSoundOscillatorControls::insertFrames(std::vector<float>& frames, unsigned int index)
{
	sampleData.insert(sampleData.begin() + index, frames.begin(), frames.end());
}

void MpcSoundOscillatorControls::insertFrames(vector<float>& left, vector<float>& right, unsigned int index)
{
	sampleData.insert(sampleData.begin() + index + getFrameCount(), right.begin(), right.end());
	sampleData.insert(sampleData.begin() + index, left.begin(), left.end());
}

MpcSoundOscillatorControls::~MpcSoundOscillatorControls()
{
}
