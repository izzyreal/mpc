#include "NextSeqScreen.hpp"
#include "sequencer/Transport.hpp"

#include "Mpc.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"

#include "sequencer/TempoChangeEvent.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"

#include "Util.hpp"

#include "StrUtil.hpp"
#include "lcdgui/Label.hpp"
#include "sequencer/SequencerStateManager.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

NextSeqScreen::NextSeqScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "next-seq", layerIndex)
{
    addReactiveBinding({[&]
                        {
                            return sequencer->getNextSq();
                        },
                        [&](const int nextSq)
                        {
                            displayNextSq();

                            if (nextSq == NoSequenceIndex)
                            {
                                selectNextSqFromScratch = true;
                            }
                        }});

    addReactiveBinding({[&]
                        {
                            return sequencer->getSelectedSequenceIndex();
                        },
                        [&](auto)
                        {
                            displaySq();
                        }});

    addReactiveBinding({[&]
                        {
                            return sequencer->getTransport()->getTickPosition();
                        },
                        [&](auto)
                        {
                            displayNow0();
                            displayNow1();
                            displayNow2();
                        }});

    addReactiveBinding({[&]
                        {
                            return sequencer->getTransport()->getTempo();
                        },
                        [&](auto)
                        {
                            displayTempo();
                        }});
}

void NextSeqScreen::open()
{
    selectNextSqFromScratch = true;

    findLabel("tempo")->setSize(12, 9);
    findField("tempo")->setLocation(18, 11);
    findField("tempo")->setLeftMargin(1);

    displaySq();
    displayNow0();
    displayNow1();
    displayNow2();
    displayTempo();
    displayTempoSource();
    displayTiming();
    displayNextSq();

    if (sequencer->getNextSq() == NoSequenceIndex)
    {
        ls->setFocus("sq");
    }
    else
    {
        ls->setFocus("nextsq");
    }
}

void NextSeqScreen::turnWheel(const int increment)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "sq")
    {
        if (sequencer->getTransport()->isPlaying())
        {
            sequencer->setNextSq(sequencer->getCurrentlyPlayingSequenceIndex() +
                                 increment);
            ls->setFocus("nextsq");
        }
        else
        {
            sequencer->getStateManager()->enqueue(
                sequencer::SetActiveSequenceIndex{
                    sequencer->getSelectedSequenceIndex() + increment});
        }
    }
    else if (focusedFieldName == "nextsq")
    {
        auto nextSq = sequencer->getNextSq();

        if (nextSq == NoSequenceIndex && increment < 0)
        {
            return;
        }

        if (nextSq == NoSequenceIndex && selectNextSqFromScratch)
        {
            nextSq = sequencer->getSelectedSequenceIndex();
            selectNextSqFromScratch = false;
        }
        else
        {
            nextSq = nextSq + increment;
        }

        sequencer->setNextSq(nextSq);

        displayNextSq();
    }
    else if (focusedFieldName == "timing")
    {
        const auto screen = mpc.screens->get<ScreenId::TimingCorrectScreen>();
        const auto noteValue = screen->getNoteValue();
        screen->setNoteValue(noteValue + increment);
        setLastFocus("timing-correct", "notevalue");
        displayTiming();
    }
    else if (focusedFieldName == "tempo")
    {
        const double oldTempo = sequencer->getTransport()->getTempo();
        const double newTempo = oldTempo + increment * 0.1;
        sequencer->getTransport()->setTempo(newTempo);
        displayTempo();
    }
}

void NextSeqScreen::function(const int i)
{
    if (i == 3 || i == 4)
    {
        const auto nextSq = sequencer->getNextSq();
        sequencer->setNextSq(NoSequenceIndex);
        selectNextSqFromScratch = true;
        displayNextSq();

        if (i == 3 && nextSq != NoSequenceIndex)
        {
            sequencer->getStateManager()->enqueue(
                sequencer::SwitchToNextSequence{nextSq});
        }
    }
    else if (i == 5)
    {
        openScreenById(ScreenId::NextSeqPadScreen);
    }
}

void NextSeqScreen::displaySq() const
{
    std::string result = "";

    if (sequencer->getTransport()->isPlaying())
    {
        result.append(StrUtil::padLeft(
            std::to_string(sequencer->getCurrentlyPlayingSequenceIndex() + 1),
            "0", 2));
        result.append("-");
        result.append(sequencer->getCurrentlyPlayingSequence()->getName());
        findField("sq")->setText(result);
    }
    else
    {
        result.append(StrUtil::padLeft(
            std::to_string(sequencer->getSelectedSequenceIndex().get() + 1),
            "0", 2));
        result.append("-");
        result.append(sequencer->getSelectedSequence()->getName());
        findField("sq")->setText(result);
    }
}

void NextSeqScreen::displayNextSq() const
{
    const auto nextSq = sequencer->getNextSq();
    std::string res = "";

    if (nextSq != NoSequenceIndex)
    {
        const auto seqName = sequencer->getSequence(nextSq)->getName();
        res = StrUtil::padLeft(std::to_string(sequencer->getNextSq() + 1), "0",
                               2) +
              "-" + seqName;
    }

    findField("nextsq")->setText(res);
}

void NextSeqScreen::displayNow0() const
{
    findField("now0")->setTextPadded(
        sequencer->getTransport()->getCurrentBarIndex() + 1, "0");
}

void NextSeqScreen::displayNow1() const
{
    findField("now1")->setTextPadded(
        sequencer->getTransport()->getCurrentBeatIndex() + 1, "0");
}

void NextSeqScreen::displayNow2() const
{
    findField("now2")->setTextPadded(
        sequencer->getTransport()->getCurrentClockNumber(), "0");
}

void NextSeqScreen::displayTempo() const
{
    displayTempoLabel();
    findField("tempo")->setText(
        Util::tempoString(sequencer->getTransport()->getTempo()));
}

void NextSeqScreen::displayTempoLabel() const
{
    auto currentRatio = -1;
    const auto sequence = sequencer->getTransport()->isPlaying()
                              ? sequencer->getCurrentlyPlayingSequence()
                              : sequencer->getSelectedSequence();
    for (const auto &e : sequence->getTempoChangeEvents())
    {
        if (e->getTick() > sequencer->getTransport()->getTickPosition())
        {
            break;
        }

        currentRatio = e->getRatio();
    }

    if (currentRatio != 1000)
    {
        findLabel("tempo")->setText(u8"c\u00C0:");
    }
    else
    {
        findLabel("tempo")->setText(u8" \u00C0:");
    }
}

void NextSeqScreen::displayTempoSource() const
{
    findField("tempo-source")
        ->setText(sequencer->getTransport()->isTempoSourceSequenceEnabled()
                      ? "(SEQ)"
                      : "(MAS)");
}

void NextSeqScreen::displayTiming() const
{
    const auto timingCorrectScreen =
        mpc.screens->get<ScreenId::TimingCorrectScreen>();
    const auto noteValue = timingCorrectScreen->getNoteValue();
    findField("timing")->setText(
        SequencerScreen::timingCorrectNames[noteValue]);
}
