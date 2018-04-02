#include <sequencer/Step.hpp>

#include <sequencer/Song.hpp>

using namespace mpc::sequencer;

Step::Step(Song* song)
{
	this->song = song;
}

void Step::setSequence(int i)
{
    if(i < 0 || i > 98) return;

    sequence = i;
    song->notifySong();
}

int Step::getSequence()
{
    return sequence;
}

void Step::setRepeats(int i)
{
    if(i < 0 || i > 99)
        return;

    repeats = i;
    song->notifySong();
}

int Step::getRepeats()
{
    return repeats;
}

Step::~Step() {
}
