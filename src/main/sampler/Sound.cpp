#include "sampler/Sound.hpp"

#include "MpcSpecs.hpp"

#include <cassert>
#include <algorithm>

using namespace mpc::sampler;

Sound::Sound(const int rate)
{
    name.reserve(Mpc2000XlSpecs::MAX_SOUND_NAME_LENGTH);
    setSampleRate(rate);
    sampleData = std::make_shared<std::vector<float>>();
}

void Sound::setName(const std::string &s)
{
    name = s;
}

void Sound::setBeatCount(const int i)
{
    numberOfBeats = std::clamp(i, 1, 32);
}

int Sound::getBeatCount() const
{
    return numberOfBeats;
}

std::string Sound::getName()
{
    return name;
}

void Sound::setLevel(const int i)
{
    if (i < 0 || i > 200)
    {
        return;
    }

    sndLevel = i;
}

void Sound::setTune(const int i)
{
    if (i < -120 || i > 120)
    {
        return;
    }

    tune = i;
}

void Sound::setLoopEnabled(const bool b)
{
    loopEnabled = b;
}

void Sound::setStart(const int i)
{
    auto value = i;
    if (value < 0)
    {
        if (start == 0)
        {
            return;
        }
        value = 0;
    }
    else if (value >= getFrameCount())
    {
        if (start == getFrameCount())
        {
            return;
        }

        value = getFrameCount();
    }

    start = value;

    if (start > end)
    {
        setEnd(start);
    }
}

void Sound::setEnd(const int i)
{
    auto value = i;

    if (value < 0)
    {

        if (end == 0)
        {
            return;
        }

        value = 0;
    }
    else if (value > getFrameCount())
    {
        if (end == getFrameCount())
        {
            return;
        }

        value = getFrameCount();
    }

    end = value;

    if (end < loopTo)
    {
        setLoopTo(end);
    }

    if (end < start)
    {
        setStart(end);
    }
}

void Sound::setMono(const bool monoToUse)
{
    mono = monoToUse;
    end = getFrameCount();
    loopTo = end;
}

void Sound::setLoopTo(const int i)
{
    auto value = i;

    if (value < 0)
    {
        value = 0;
    }
    else if (value > getFrameCount())
    {
        value = getFrameCount();
    }

    loopTo = value;
}

void Sound::setSampleData(
    const std::shared_ptr<std::vector<float>> &newSampleData)
{
    sampleData = newSampleData;
}

int Sound::getLastFrameIndex() const
{
    return (isMono() ? sampleData->size() : sampleData->size() * 0.5) - 1;
}

int Sound::getFrameCount() const
{
    return getLastFrameIndex() + 1;
}

int Sound::getTune() const
{
    return tune;
}

bool Sound::isLoopEnabled() const
{
    return loopEnabled;
}

int Sound::getStart() const
{
    return start;
}

int Sound::getEnd() const
{
    return end;
}

std::shared_ptr<const std::vector<float>> Sound::getSampleData() const
{
    return sampleData;
}

std::shared_ptr<std::vector<float>> Sound::getMutableSampleData() const
{
    return sampleData;
}

bool Sound::isMono() const
{
    return mono;
}

int Sound::getLoopTo() const
{
    return loopTo;
}

int Sound::getSampleRate() const
{
    return sampleRate;
}

void Sound::setSampleRate(const int sr)
{
    sampleRate = sr;
}

int Sound::getSndLevel() const
{
    return sndLevel;
}

void Sound::insertFrame(const std::vector<float> &frame,
                        const unsigned int index) const
{
    if (index > getFrameCount())
    {
        return;
    }

    if (!mono)
    {
        if (frame.size() < 2)
        {
            return;
        }
        const unsigned int rightIndex = index + getFrameCount();
        sampleData->insert(sampleData->begin() + rightIndex, frame[1]);
    }

    if (frame.size() < 1)
    {
        return;
    }

    sampleData->insert(sampleData->begin() + index, frame[0]);
}

void Sound::insertFrames(std::vector<float> &frames, const unsigned int index,
                         const uint32_t nFrames) const
{
    assert(mono);
    sampleData->insert(sampleData->begin() + index, frames.begin(),
                       frames.begin() + nFrames);
}

void Sound::insertFrames(std::vector<float> &left, std::vector<float> &right,
                         const unsigned int index, const uint32_t nFrames) const
{
    assert(!mono);
    sampleData->insert(sampleData->begin() + index + getFrameCount(),
                       right.begin(), right.begin() + nFrames);
    sampleData->insert(sampleData->begin() + index, left.begin(),
                       left.begin() + nFrames);
}

void Sound::appendFrames(std::vector<float> &frames,
                         const uint32_t nFrames) const
{
    insertFrames(frames, getFrameCount(), nFrames);
}

void Sound::appendFrames(std::vector<float> &left, std::vector<float> &right,
                         const uint32_t nFrames) const
{
    insertFrames(left, right, getFrameCount(), nFrames);
}

void Sound::removeFramesFromEnd(const int numFramesToRemove) const
{
    if (!mono)
    {
        sampleData->erase(sampleData->end() - getFrameCount() -
                              numFramesToRemove,
                          sampleData->end() - getFrameCount());
    }

    sampleData->erase(sampleData->end() - numFramesToRemove, sampleData->end());
}
