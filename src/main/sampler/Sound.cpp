#include <sampler/Sound.hpp>

#include <cassert>

using namespace mpc::sampler;

Sound::Sound(int rate)
{
    setSampleRate(rate);
}

void Sound::setName(std::string s)
{
    name = s;
    
    notifyObservers(std::string("samplename"));
}

void Sound::setBeatCount(int i)
{
    if(i < 1 || i > 32)
        return;

    numberOfBeats = i;
    
    notifyObservers(std::string("beat"));
}

int Sound::getBeatCount()
{
    return numberOfBeats;
}

std::string Sound::getName()
{
    return name;
}

void Sound::setLevel(int i)
{
    if (i < 0 || i > 200) return;

    sndLevel = i;
}

void Sound::setTune(int i)
{
    if (i < -120 || i > 120) return;

    tune = i;
}

void Sound::setLoopEnabled(bool b)
{
    loopEnabled = b;
}

void Sound::setStart(int i)
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

void Sound::setEnd(int i)
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

void Sound::setMono(bool b)
{
    mono = b;
    end = getFrameCount();
    loopTo = end;
}

void Sound::setLoopTo(int i)
{
    auto value = i;

    if (value < 0)
        value = 0;
    else if (value > getFrameCount())
        value = getFrameCount();

    loopTo = value;
}


int Sound::getLastFrameIndex()
{
    return (isMono() ? sampleData.size() : (sampleData.size() * 0.5)) - 1;
}

int Sound::getFrameCount()
{
    return getLastFrameIndex() + 1;
}

int Sound::getTune()
{
    return tune;
}

bool Sound::isLoopEnabled()
{
    return loopEnabled;
}

int Sound::getStart()
{
    return start;
}

int Sound::getEnd()
{
    return end;
}

std::vector<float>* Sound::getSampleData()
{
    return &sampleData;
}

bool Sound::isMono()
{
    return mono;
}

int Sound::getLoopTo()
{
    return loopTo;
}

int Sound::getSampleRate()
{
    return sampleRate;
}

void Sound::setSampleRate(int sr)
{
    sampleRate = sr;
}

int Sound::getSndLevel()
{
    return sndLevel;
}

void Sound::insertFrame(std::vector<float> frame, unsigned int index)
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

void Sound::insertFrames(std::vector<float>& frames, unsigned int index, uint32_t nFrames)
{
    assert(mono);
    sampleData.insert(sampleData.begin() + index, frames.begin(), nFrames == 0 ? frames.end() : frames.begin() + nFrames);
}

void Sound::insertFrames(std::vector<float>& left, std::vector<float>& right, unsigned int index, uint32_t nFrames)
{
    assert(!mono);
    sampleData.insert(sampleData.begin() + index + getFrameCount(), right.begin(), nFrames == 0 ? right.end() : right.begin() + nFrames);
    sampleData.insert(sampleData.begin() + index, left.begin(), nFrames == 0 ? left.end() : left.begin() + nFrames);
}

void Sound::appendFrames(std::vector<float> &frames, uint32_t nFrames)
{
    insertFrames(frames, getFrameCount(), nFrames);
}

void Sound::appendFrames(std::vector<float> &left, std::vector<float> &right, uint32_t nFrames)
{
    insertFrames(left, right, getFrameCount(), nFrames);
}

void Sound::removeFramesFromEnd(int numFramesToRemove)
{
    if (!mono)
    {
        sampleData.erase(sampleData.end() - getFrameCount() - numFramesToRemove, sampleData.end() - getFrameCount());
    }

    sampleData.erase(sampleData.end() - numFramesToRemove, sampleData.end());
}
