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
                            return sequencer.lock()->getNextSq();
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
                            return sequencer.lock()->getSelectedSequenceIndex();
                        },
                        [&](auto)
                        {
                            displaySq();
                        }});

    addReactiveBinding(
        {[&]
         {
             return sequencer.lock()->getTransport()->getTickPosition();
         },
         [&](auto)
         {
             displayNow0();
             displayNow1();
             displayNow2();
         }});

    addReactiveBinding({[&]
                        {
                            return sequencer.lock()->getTransport()->getTempo();
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

    if (sequencer.lock()->getNextSq() == NoSequenceIndex)
    {
        ls.lock()->setFocus("sq");
    }
    else
    {
        ls.lock()->setFocus("nextsq");
    }
}

void NextSeqScreen::turnWheel(const int increment)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "sq")
    {
        if (sequencer.lock()->getTransport()->isPlaying())
        {
            sequencer.lock()->setNextSq(
                sequencer.lock()->getCurrentlyPlayingSequenceIndex() +
                increment);
            ls.lock()->setFocus("nextsq");
        }
        else
        {
            sequencer.lock()->getStateManager()->enqueue(
                sequencer::SetSelectedSequenceIndex{
                    sequencer.lock()->getSelectedSequenceIndex() + increment});
        }
    }
    else if (focusedFieldName == "nextsq")
    {
        auto nextSq = sequencer.lock()->getNextSq();

        if (nextSq == NoSequenceIndex && increment < 0)
        {
            return;
        }

        if (nextSq == NoSequenceIndex && selectNextSqFromScratch)
        {
            nextSq = sequencer.lock()->getSelectedSequenceIndex();
            selectNextSqFromScratch = false;
        }
        else
        {
            nextSq = nextSq + increment;
        }

        sequencer.lock()->setNextSq(nextSq);

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
        const double oldTempo = sequencer.lock()->getTransport()->getTempo();
        const double newTempo = oldTempo + increment * 0.1;
        sequencer.lock()->getTransport()->setTempo(newTempo);
        displayTempo();
    }
}

void NextSeqScreen::function(const int i)
{
    if (i == 3 || i == 4)
    {
        const auto nextSq = sequencer.lock()->getNextSq();
        sequencer.lock()->setNextSq(NoSequenceIndex);
        selectNextSqFromScratch = true;
        displayNextSq();

        if (i == 3 && nextSq != NoSequenceIndex)
        {
            sequencer.lock()->getStateManager()->enqueue(
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

    if (sequencer.lock()->getTransport()->isPlaying())
    {
        result.append(StrUtil::padLeft(
            std::to_string(
                sequencer.lock()->getCurrentlyPlayingSequenceIndex() + 1),
            "0", 2));
        result.append("-");
        result.append(
            sequencer.lock()->getCurrentlyPlayingSequence()->getName());
        findField("sq")->setText(result);
    }
    else
    {
        result.append(StrUtil::padLeft(
            std::to_string(sequencer.lock()->getSelectedSequenceIndex().get() +
                           1),
            "0", 2));
        result.append("-");
        result.append(sequencer.lock()->getSelectedSequence()->getName());
        findField("sq")->setText(result);
    }
}

void NextSeqScreen::displayNextSq() const
{
    const auto nextSq = sequencer.lock()->getNextSq();
    std::string res = "";

    if (nextSq != NoSequenceIndex)
    {
        const auto seqName = sequencer.lock()->getSequence(nextSq)->getName();
        res = StrUtil::padLeft(
                  std::to_string(sequencer.lock()->getNextSq() + 1), "0", 2) +
              "-" + seqName;
    }

    findField("nextsq")->setText(res);
}

void NextSeqScreen::displayNow0() const
{
    findField("now0")->setTextPadded(
        sequencer.lock()->getTransport()->getCurrentBarIndex() + 1, "0");
}

void NextSeqScreen::displayNow1() const
{
    findField("now1")->setTextPadded(
        sequencer.lock()->getTransport()->getCurrentBeatIndex() + 1, "0");
}

void NextSeqScreen::displayNow2() const
{
    findField("now2")->setTextPadded(
        sequencer.lock()->getTransport()->getCurrentClockNumber(), "0");
}

void NextSeqScreen::displayTempo() const
{
    displayTempoLabel();
    findField("tempo")->setText(
        Util::tempoString(sequencer.lock()->getTransport()->getTempo()));
}

void NextSeqScreen::displayTempoLabel() const
{
    auto currentRatio = -1;
    const auto sequence = sequencer.lock()->getTransport()->isPlaying()
                              ? sequencer.lock()->getCurrentlyPlayingSequence()
                              : sequencer.lock()->getSelectedSequence();
    for (const auto &e : sequence->getTempoChangeEvents())
    {
        if (e->getTick() > sequencer.lock()->getTransport()->getTickPosition())
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
        ->setText(
            sequencer.lock()->getTransport()->isTempoSourceSequenceEnabled()
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
