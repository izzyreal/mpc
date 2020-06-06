#include <sequencer/Song.hpp>

#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/Step.hpp>

using namespace mpc::sequencer;
using namespace std;

Song::Song(Sequencer* sequencer) 
{
	this->sequencer = sequencer;
}

void Song::setLoopEnabled(bool b)
{
    loopEnabled = b;
}

bool Song::isLoopEnabled()
{
    return loopEnabled;
}

void Song::setFirstStep(int i)
{
    if(i < 0 || i > (int) (steps.size()) -1)
        return;

    firstStep = i;
    if(firstStep > lastStep)
        setLastStep(firstStep);

    setChanged();
	notifyObservers(string("firststep"));
}

int Song::getFirstStep()
{
    return firstStep;
}

void Song::setLastStep(int i)
{
	if (i < 0 || i >(int) (steps.size()) - 1)
		return;

	lastStep = i;
	if (lastStep < firstStep)
		setFirstStep(lastStep);

	setChanged();
	notifyObservers(string("laststep"));
}

int Song::getLastStep()
{
    return lastStep;
}

void Song::setName(string str)
{
    name = str;
    setChanged();
	notifyObservers(string("song-name"));
}

string Song::getName()
{
	if (!used) return "(Unused)";
	return name;
}

void Song::deleteStep(int stepIndex)
{
    if(stepIndex > (int) (steps.size()) - 1) return;

    steps.erase(steps.begin() + stepIndex);
    notifySong();
}

void Song::insertStep(int stepIndex, Step* s)
{
	steps.insert(steps.begin() + stepIndex, s);
	if (!used) {
		used = true;
		setChanged();
		notifyObservers(string("song-name"));
	}
	notifySong();
}

void Song::notifySong()
{
    setChanged();
	notifyObservers(string("song"));
}

void Song::setStep(int i, Step* s)
{
	if (steps[i] != nullptr) {
		delete steps[i];
		steps[i] = nullptr;
	}
	steps[i] = s;
	notifySong();
}

Step* Song::getStep(int i)
{
	return steps[i];
}

int Song::getStepAmount()
{
    return steps.size();
}

int Song::getStepBarAmount(int step)
{
    auto seq = sequencer->getSequence(steps[step]->getSequence()).lock();
    if(!seq->isUsed()) return 0;
    return seq->getLastBar();
}

bool Song::isUsed()
{
    return used;
}

void Song::setUsed(bool b)
{
    used = true;
    setChanged();
	notifyObservers(string("used"));
}

Song::~Song() {
	for (auto& s : steps)
		delete s;
}
