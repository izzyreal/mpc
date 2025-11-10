#include "Transport.hpp"
#include "Sequencer.hpp"

using namespace mpc::sequencer;

Transport::Transport(Sequencer &owner) : sequencer(owner) {}

void Transport::resetCountInPositions()
{
    countInStartPos = -1;
    countInEndPos = -1;
}

void Transport::play()
{
}

void Transport::playFromStart()
{
}

void Transport::rec()
{
}

void Transport::recFromStart()
{
}

void Transport::overdub()
{
}

void Transport::overdubFromStart()
{
}

void Transport::switchRecordToOverdub() {
}

void Transport::stop()
{
}

void Transport::stop(const StopMode)
{
}

bool Transport::isPlaying() const
{
    return playing;
}

bool Transport::isRecording() const
{
    return recording;
}

bool Transport::isOverdubbing() const
{
    return overdubbing;
}

bool Transport::isRecordingOrOverdubbing() const
{
    return recording || overdubbing;
}

void Transport::setRecording(bool b)
{
    recording = b;
}

void Transport::setOverdubbing(bool b)
{
    overdubbing = b;
}

bool Transport::isCountingIn() const
{
    return countingIn;
}

void Transport::setCountingIn(bool b)
{
    countingIn = b;
}

void Transport::move(double) {}

void Transport::moveWithinSong(double) {}

void Transport::setPosition(double) {}

void Transport::setPositionWithinSong(double) {}

void Transport::bumpPositionByTicks(uint8_t) {}

int Transport::getTickPosition() const
{
    return 0;
}

double Transport::getPlayStartPositionQuarterNotes() const
{
    return 0.0;
}

void Transport::playMetronomeTrack() {}

void Transport::stopMetronomeTrack() {}

bool Transport::isPunchEnabled() const
{
    return punchEnabled;
}

void Transport::setPunchEnabled(bool enabled)
{
    punchEnabled = enabled;
}

int Transport::getAutoPunchMode() const
{
    return autoPunchMode;
}

void Transport::setAutoPunchMode(int mode)
{
    autoPunchMode = mode;
}

int Transport::getPunchInTime() const
{
    return punchInTime;
}

void Transport::setPunchInTime(int time)
{
    punchInTime = time;
}

int Transport::getPunchOutTime() const
{
    return punchOutTime;
}

void Transport::setPunchOutTime(int time)
{
    punchOutTime = time;
}

int64_t Transport::getCountInStartPos() const
{
    return countInStartPos;
}

int64_t Transport::getCountInEndPos() const
{
    return countInEndPos;
}

