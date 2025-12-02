#include "TimingCorrectScreen.hpp"

#include "sampler/Sampler.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/SeqUtil.hpp"

#include <Util.hpp>

#include "StrUtil.hpp"
#include "lcdgui/FunctionKeys.hpp"
#include "lcdgui/Label.hpp"
#include "sequencer/NoteOnEvent.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;

TimingCorrectScreen::TimingCorrectScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "timing-correct", layerIndex)
{
}

void TimingCorrectScreen::open()
{
    const auto track = sequencer.lock()->getSelectedTrack();

    if (track->getBusType() != BusType::MIDI)
    {
        note0 = DrumNoteNumber(note0);
    }

    findField("note1")->setAlignment(Alignment::Centered, 18);
    findField("note1")->setLocation(116, 40);

    const auto seq = sequencer.lock()->getSelectedSequence();

    setTime0(0);
    setTime1(seq->getLastTick());

    displayNoteValue();
    displaySwing();
    displayShiftTiming();
    displayAmount();
    displayTime();
    displayNotes();
}

void TimingCorrectScreen::function(int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 4:
        {
            sequencer.lock()->storeSelectedSequenceInUndoPlaceHolder();

            std::vector<int> noteRange(2);

            const auto track = sequencer.lock()->getSelectedTrack();

            if (track->getBusType() != BusType::MIDI)
            {
                if (note0 == NoDrumNoteAssigned)
                {
                    noteRange[0] = MinNoteNumber;
                    noteRange[1] = MaxNoteNumber;
                }
                else
                {
                    noteRange[0] = note0;
                    noteRange[1] = note0;
                }
            }
            else
            {
                noteRange[0] = note0;
                noteRange[1] = note1;
            }

            const auto eventRange = track->getEventRange(time0, time1);

            const auto sequence = sequencer.lock()->getSelectedSequence();

            for (auto &e : eventRange)
            {
                if (const auto noteEvent =
                        std::dynamic_pointer_cast<NoteOnEvent>(e))
                {
                    if (noteEvent->getNote() >= noteRange[0] &&
                        noteEvent->getNote() <= noteRange[1])
                    {
                        track->shiftTiming(e->handle, e->getTick(),
                                           shiftTimingLater, amount,
                                           sequence->getLastTick());
                    }
                }
            }

            track->correctTimeRange(time0, time1, getNoteValueLengthInTicks(),
                                    swing, noteRange[0], noteRange[1]);

            openScreenById(ScreenId::SequencerScreen);
            break;
        }
        default:;
    }
}

void TimingCorrectScreen::turnWheel(int i)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "notevalue")
    {
        setNoteValue(noteValue.load() + i);
    }
    else if (focusedFieldName == "swing")
    {
        setSwing(swing + i);
    }
    else if (focusedFieldName == "shifttiming")
    {
        setShiftTimingLater(i > 0);
    }
    else if (focusedFieldName == "amount")
    {
        setAmount(amount + i);
    }

    checkAllTimesAndNotes(mpc, i);
    displayTime();
}

void TimingCorrectScreen::displayNoteValue()
{
    const auto currentNoteValue = noteValue.load();
    findChild<FunctionKey>("fk4")->Hide(currentNoteValue == 0);
    SetDirty();

    findField("notevalue")->setText(noteValueNames[currentNoteValue]);
    findLabel("swing")->Hide(!(currentNoteValue == 1 || currentNoteValue == 3));
    findField("swing")->Hide(!(currentNoteValue == 1 || currentNoteValue == 3));
}

void TimingCorrectScreen::displaySwing() const
{
    findField("swing")->setText(std::to_string(swing));
}

void TimingCorrectScreen::displayNotes()
{
    const auto track = sequencer.lock()->getSelectedTrack();

    if (track->getBusType() == BusType::MIDI)
    {
        findField("note0")->setAlignment(Alignment::Centered, 18);
        findField("note0")->setLocation(62, 40);
        findField("note0")->setSize(47, 9);
        findField("note0")->setText(
            StrUtil::padLeft(std::to_string(note0), " ", 3) + "(" +
            mpc::Util::noteNames()[note0] + u8"\u00D4");
        findField("note1")->setText(
            StrUtil::padLeft(std::to_string(note1), " ", 3) + "(" +
            mpc::Util::noteNames()[note1] + u8"\u00D4");
        findLabel("note1")->Hide(false);
        findField("note1")->Hide(false);
    }
    else
    {
        findField("note0")->setAlignment(Alignment::None);
        findField("note0")->setLocation(61, 40);
        findField("note0")->setSize(37, 9);

        if (note0 == NoDrumNoteAssigned)
        {
            findField("note0")->setText("ALL");
        }
        else
        {
            const auto program = getProgramOrThrow();
            const auto padIndex =
                program->getPadIndexFromNote(DrumNoteNumber(note0));
            const auto padName = sampler.lock()->getPadName(padIndex);
            findField("note0")->setText(std::to_string(note0) + "/" + padName);
        }

        findLabel("note1")->Hide(true);
        findField("note1")->Hide(true);
    }
}

void TimingCorrectScreen::displayShiftTiming() const
{
    findField("shifttiming")->setText(shiftTimingLater ? "LATER" : "EARLIER");
}

void TimingCorrectScreen::displayAmount() const
{
    findField("amount")->setTextPadded(amount);
}

void TimingCorrectScreen::displayTime()
{
    const auto s = sequencer.lock()->getSelectedSequence().get();
    findField("time0")->setTextPadded(SeqUtil::getBarFromTick(s, time0) + 1,
                                      "0");
    findField("time1")->setTextPadded(SeqUtil::getBeat(s, time0) + 1, "0");
    findField("time2")->setTextPadded(SeqUtil::getClock(s, time0), "0");
    findField("time3")->setTextPadded(SeqUtil::getBarFromTick(s, time1) + 1,
                                      "0");
    findField("time4")->setTextPadded(SeqUtil::getBeat(s, time1) + 1, "0");
    findField("time5")->setTextPadded(SeqUtil::getClock(s, time1), "0");
}

void TimingCorrectScreen::setAmount(int i)
{
    int maxVal = 0;

    switch (noteValue.load())
    {
        case 0:
            maxVal = 0;
            break;
        case 1:
            maxVal = 23;
            break;
        case 2:
            maxVal = 15;
            break;
        case 3:
            maxVal = 11;
            break;
        case 4:
            maxVal = 7;
            break;
        case 5:
            maxVal = 5;
            break;
        case 6:
            maxVal = 3;
            break;
        default:
            maxVal = 0;
            break;
    }

    amount = std::clamp(i, 0, maxVal);
    displayAmount();
}

void TimingCorrectScreen::setShiftTimingLater(bool b)
{
    shiftTimingLater = b;
    displayShiftTiming();
}

int TimingCorrectScreen::getSwing() const
{
    return swing;
}

void TimingCorrectScreen::setSwing(int i)
{
    swing = std::clamp(i, 50, 75);
    displaySwing();
}

unsigned char TimingCorrectScreen::getNoteValueLengthInTicks() const
{
    return noteValueLengthsInTicks[noteValue.load()];
}

int TimingCorrectScreen::getNoteValue() const
{
    return noteValue.load();
}

void TimingCorrectScreen::setNoteValue(int i)
{
    noteValue.store(std::clamp(i, 0, 6));
    setAmount(amount); // reclamp to new bounds
    displayNoteValue();
}

int TimingCorrectScreen::getAmount() const
{
    return amount;
}

int TimingCorrectScreen::isShiftTimingLater() const
{
    return shiftTimingLater;
}
