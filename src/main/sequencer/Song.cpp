#include <sequencer/Song.hpp>

#include <sequencer/Step.hpp>

using namespace mpc::sequencer;
using namespace std;

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
    if (i < 0 || i >= (int) steps.size())
        return;

    firstStep = i;

    if (firstStep > lastStep)
        setLastStep(firstStep);
}

int Song::getFirstStep()
{
    return firstStep;
}

void Song::setLastStep(int i)
{
	if (i < 0 || i >= (int) steps.size())
		return;

	lastStep = i;

	if (lastStep < firstStep)
		setFirstStep(lastStep);	
}

int Song::getLastStep()
{
    return lastStep;
}

void Song::setName(string str)
{
    name = str;
}

string Song::getName()
{
	if (!used)
        return "(Unused)";
	
    return name;
}

void Song::deleteStep(int stepIndex)
{
    if (stepIndex >= (int) steps.size())
		return;

    steps.erase(steps.begin() + stepIndex);
}

void Song::insertStep(int stepIndex)
{
	steps.insert(steps.begin() + stepIndex, make_shared<Step>());
}

weak_ptr<Step> Song::getStep(int i)
{
	return steps[i];
}

int Song::getStepCount()
{
    return steps.size();
}

bool Song::isUsed()
{
    return used;
}

void Song::setUsed(bool b)
{
    used = true;
}
