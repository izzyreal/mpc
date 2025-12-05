#include "lcdgui/screens/WithLocateStepEventBarSequence.hpp"

#include "hardware/Component.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Transport.hpp"

using namespace mpc::lcdgui::screens;

WithLocateStepEventBarSequence::WithLocateStepEventBarSequence(
    hardware::Button *goToButton, sequencer::Sequencer *sequencer)
    : goToButton(goToButton), mySequencer(sequencer)
{
}

void WithLocateStepEventBarSequence::prevStepEvent()
{
    if (goToButton->isPressed())
    {
        mySequencer->goToPreviousEvent();
    }
    else
    {
        mySequencer->goToPreviousStep();
    }
}

void WithLocateStepEventBarSequence::nextStepEvent()
{
    if (goToButton->isPressed())
    {
        mySequencer->goToNextEvent();
    }
    else
    {
        mySequencer->goToNextStep();
    }
}

void WithLocateStepEventBarSequence::prevBarStart()
{
    if (goToButton->isPressed())
    {
        mySequencer->getTransport()->setPosition(0);
    }
    else
    {
        mySequencer->getTransport()->setBar(
            mySequencer->getTransport()->getCurrentBarIndex() - 1);
    }
}

void WithLocateStepEventBarSequence::nextBarEnd()
{
    if (goToButton->isPressed())
    {
        mySequencer->getTransport()->setBar(
            mySequencer->getSelectedSequence()->getLastBarIndex() + 1);
    }
    else
    {
        mySequencer->getTransport()->setBar(
            mySequencer->getTransport()->getCurrentBarIndex() + 1);
    }
}
