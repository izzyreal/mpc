#include "NextSeqPadScreen.hpp"

#include "StrUtil.hpp"
#include "controller/ClientEventController.hpp"
#include "hardware/Hardware.hpp"
#include "lcdgui/Label.hpp"

#include "Mpc.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::lcdgui::screens;

NextSeqPadScreen::NextSeqPadScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "next-seq-pad", layerIndex)
{
    addReactiveBinding({[&]
                        {
                            return sequencer->getNextSq();
                        },
                        [&](auto)
                        {
                            displaySq();
                            displayNextSq();
                            refreshSeqs();
                        }});

    addReactiveBinding({[&]
                        {
                            return mpc.clientEventController->getActiveBank();
                        },
                        [&](auto)
                        {
                            displayBank();
                            displaySeqNumbers();
                            refreshSeqs();
                        }});

    addReactiveBinding({[&]
                        {
                            return sequencer->isSoloEnabled();
                        },
                        [&](auto)
                        {
                            refreshSeqs();
                        }});

    addReactiveBinding({[&]
                        {
                            return sequencer->getActiveSequenceIndex();
                        },
                        [&](auto)
                        {
                            displaySq();
                            refreshSeqs();
                        }});

    addReactiveBinding({[&]
                        {
                            return sequencer->getTickPosition();
                        },
                        [&](auto)
                        {
                            displayNow0();
                            displayNow1();
                            displayNow2();
                        }});
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
}

void NextSeqPadScreen::right()
{
    // Block ScreenComponent::right() default action. Nothing to do.
}

void NextSeqPadScreen::function(int i)
{

    if (i == 3 || i == 4)
    {
        const auto nextSq = sequencer->getNextSq();
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
        openScreenById(ScreenId::NextSeqScreen);
    }
}

void NextSeqPadScreen::displayNextSq() const
{
    const auto nextSq = sequencer->getNextSq();

    if (nextSq == -1)
    {
        findLabel("nextsq")->setText(" ");
        return;
    }

    const auto number = StrUtil::padLeft(std::to_string(nextSq + 1), "0", 2);
    const auto name = sequencer->getSequence(nextSq)->getName();
    findLabel("nextsq")->setText(number + "-" + name);
}

int NextSeqPadScreen::bankOffset() const
{
    return static_cast<int>(mpc.clientEventController->getActiveBank()) * 16;
}

void NextSeqPadScreen::displayBank() const
{
    findLabel("bank")->setText(
        letters[static_cast<int>(mpc.clientEventController->getActiveBank())]);
}

void NextSeqPadScreen::displaySeqNumbers() const
{
    const std::vector<std::string> seqn{"01-16", "17-32", "33-48", "49-64"};
    findLabel("seqnumbers")
        ->setText(
            seqn[static_cast<int>(mpc.clientEventController->getActiveBank())]);
}

void NextSeqPadScreen::displaySq() const
{
    findField("sq")->setText(
        StrUtil::padLeft(
            std::to_string(sequencer->getActiveSequenceIndex() + 1), "0", 2) +
        "-" + sequencer->getActiveSequence()->getName());
}

void NextSeqPadScreen::displaySeq(int i) const
{
    findField(std::to_string(i + 1))
        ->setText(
            sequencer->getSequence(i + bankOffset())->getName().substr(0, 8));
}

void NextSeqPadScreen::setSeqColor(int i) const
{
    findField(std::to_string(i + 1))
        ->setInverted(i + bankOffset() == sequencer->getNextSq());
}

void NextSeqPadScreen::displayNow0() const
{
    findField("now0")->setTextPadded(sequencer->getCurrentBarIndex() + 1, "0");
}

void NextSeqPadScreen::displayNow1() const
{
    findField("now1")->setTextPadded(sequencer->getCurrentBeatIndex() + 1, "0");
}

void NextSeqPadScreen::displayNow2() const
{
    findField("now2")->setTextPadded(sequencer->getCurrentClockNumber(), "0");
}

void NextSeqPadScreen::refreshSeqs() const
{
    for (int i = 0; i < 16; i++)
    {
        displaySeq(i);
        setSeqColor(i);
    }
}
