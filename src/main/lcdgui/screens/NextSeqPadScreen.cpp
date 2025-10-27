#include "NextSeqPadScreen.hpp"

#include "controller/ClientEventController.hpp"
#include "hardware/Hardware.hpp"

using namespace mpc::lcdgui::screens;

NextSeqPadScreen::NextSeqPadScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "next-seq-pad", layerIndex)
{
}

void NextSeqPadScreen::open()
{
    for (int i = 0; i < 16; i++)
    {
        findField(std::to_string(i + 1))->setFocusable(false);
        displaySeq(i);
        setSeqColor(i);
    }

    displaySq();
    displayNow0();
    displayNow1();
    displayNow2();
    displayBank();
    displaySeqNumbers();
    displayNextSq();

    sequencer->addObserver(this);
    mpc.clientEventController->addObserver(this);
}

void NextSeqPadScreen::close()
{
    sequencer->deleteObserver(this);
    mpc.clientEventController->deleteObserver(this);
}

void NextSeqPadScreen::right()
{
    // Block ScreenComponent::right() default action. Nothing to do.
}

void NextSeqPadScreen::function(int i)
{

    if (i == 3 || i == 4)
    {
        auto nextSq = sequencer->getNextSq();
        sequencer->setNextSq(-1);
        displayNextSq();

        if (i == 3 && nextSq != -1)
        {
            sequencer->stop();
            sequencer->move(0);
            sequencer->setActiveSequenceIndex(nextSq);
            sequencer->playFromStart();
        }
    }
    else if (i == 5)
    {
        mpc.getLayeredScreen()->openScreen<NextSeqScreen>();
    }
}

void NextSeqPadScreen::displayNextSq()
{
    auto nextSq = sequencer->getNextSq();

    if (nextSq == -1)
    {
        findLabel("nextsq")->setText(" ");
        return;
    }

    auto number = StrUtil::padLeft(std::to_string(nextSq + 1), "0", 2);
    auto name = sequencer->getSequence(nextSq)->getName();
    findLabel("nextsq")->setText(number + "-" + name);
}

int NextSeqPadScreen::bankOffset()
{
    return static_cast<int>(mpc.clientEventController->getActiveBank()) * 16;
}

void NextSeqPadScreen::displayBank()
{
    findLabel("bank")->setText(letters[static_cast<int>(mpc.clientEventController->getActiveBank())]);
}

void NextSeqPadScreen::displaySeqNumbers()
{
    std::vector<std::string> seqn{"01-16", "17-32", "33-48", "49-64"};
    findLabel("seqnumbers")->setText(seqn[static_cast<int>(mpc.clientEventController->getActiveBank())]);
}

void NextSeqPadScreen::displaySq()
{
    findField("sq")->setText(StrUtil::padLeft(std::to_string(sequencer->getActiveSequenceIndex() + 1), "0", 2) + "-" + sequencer->getActiveSequence()->getName());
}

void NextSeqPadScreen::displaySeq(int i)
{
    findField(std::to_string(i + 1))->setText(sequencer->getSequence(i + bankOffset())->getName().substr(0, 8));
}

void NextSeqPadScreen::setSeqColor(int i)
{
    findField(std::to_string(i + 1))->setInverted(i + bankOffset() == sequencer->getNextSq());
}

void NextSeqPadScreen::displayNow0()
{
    findField("now0")->setTextPadded(sequencer->getCurrentBarIndex() + 1, "0");
}

void NextSeqPadScreen::displayNow1()
{
    findField("now1")->setTextPadded(sequencer->getCurrentBeatIndex() + 1, "0");
}

void NextSeqPadScreen::displayNow2()
{
    findField("now2")->setTextPadded(sequencer->getCurrentClockNumber(), "0");
}

void NextSeqPadScreen::refreshSeqs()
{
    for (int i = 0; i < 16; i++)
    {
        displaySeq(i);
        setSeqColor(i);
    }
}

void NextSeqPadScreen::update(Observable *observable, Message message)
{
    const auto msg = std::get<std::string>(message);
    if (msg == "bank")
    {
        displayBank();
        displaySeqNumbers();

        for (int i = 0; i < 16; i++)
        {
            displaySeq(i);
        }
    }
    else if (msg == "soloenabled")
    {
        refreshSeqs();
    }
    else if (msg == "seqnumbername")
    {
        displaySq();
        refreshSeqs();
    }
    else if (msg == "nextsqoff")
    {
        refreshSeqs();
        displayNextSq();
    }
    else if (msg == "nextsqvalue" || msg == "nextsq")
    {
        refreshSeqs();
        displayNextSq();
    }
    else if (msg == "now" || msg == "clock")
    {
        displayNow0();
        displayNow1();
        displayNow2();
    }
}
