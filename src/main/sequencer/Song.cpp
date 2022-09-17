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
    if (i >= steps.size()) i = steps.size() - 1;
    if (i < 0) i = 0;

    if (firstStep == i)
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
    if (i >= steps.size()) i = steps.size() - 1;

    if (i < 0) i = 0;

    if (lastStep == i)
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

    if (lastStep >= steps.size())
        setLastStep(steps.size() - 1);
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
    used = b;

    if (!used)
    {
        name = "";
        steps.clear();
    }
}
