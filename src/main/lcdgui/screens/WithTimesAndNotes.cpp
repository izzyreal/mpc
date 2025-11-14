#include "WithTimesAndNotes.hpp"

#include "Mpc.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/SeqUtil.hpp"

#include "lcdgui/ScreenComponent.hpp"

using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens;

bool WithTimesAndNotes::checkAllTimes(Mpc &mpc, const int notch,
                                      Sequence *sequence)
{
    const auto sequenceToUse =
        sequence != nullptr ? sequence
                            : mpc.getSequencer()->getActiveSequence().get();

    const auto focusedFieldName = mpc.getLayeredScreen()
                                      ->getCurrentScreen()
                                      ->getFocusedFieldNameOrThrow();

    if (focusedFieldName == "time0")
    {
        setTime0(SeqUtil::getTickFromBar(
            SeqUtil::getBarFromTick(sequenceToUse, time0) + notch,
            sequenceToUse, time0));
        return true;
    }
    if (focusedFieldName == "time1")
    {
        setTime0(
            SeqUtil::setBeat(SeqUtil::getBeat(sequenceToUse, time0) + notch,
                             sequenceToUse, time0));
        return true;
    }
    if (focusedFieldName == "time2")
    {
        setTime0(
            SeqUtil::setClock(SeqUtil::getClock(sequenceToUse, time0) + notch,
                              sequenceToUse, time0));
        return true;
    }
    if (focusedFieldName == "time3")
    {
        setTime1(SeqUtil::getTickFromBar(
            SeqUtil::getBarFromTick(sequenceToUse, time1) + notch,
            sequenceToUse, time1));
        return true;
    }
    if (focusedFieldName == "time4")
    {
        setTime1(
            SeqUtil::setBeat(SeqUtil::getBeat(sequenceToUse, time1) + notch,
                             sequenceToUse, time1));
        return true;
    }
    if (focusedFieldName == "time5")
    {
        setTime1(
            SeqUtil::setClock(SeqUtil::getClock(sequenceToUse, time1) + notch,
                              sequenceToUse, time1));
        return true;
    }

    return false;
}

bool WithTimesAndNotes::checkAllTimesAndNotes(Mpc &mpc, const int notch,
                                              Sequence *sequence,
                                              const Track *track)
{
    auto param = mpc.getLayeredScreen()->getFocusedFieldName();

    const auto timesHaveChanged = checkAllTimes(mpc, notch, sequence);
    auto notesHaveChanged = false;

    const auto focusedFieldName = mpc.getLayeredScreen()
                                      ->getCurrentScreen()
                                      ->getFocusedFieldNameOrThrow();

    if (focusedFieldName == "note0")
    {

        if (const auto trackToUse =
                track != nullptr ? track
                                 : mpc.getSequencer()->getActiveTrack().get();
            isMidiBusType(trackToUse->getBusType()))
        {
            const auto note = note0 + notch;

            if (note < 34 || note > 98)
            {
                return true;
            }

            note0 = note;

            displayDrumNotes();
        }
        else
        {
            setNote0(note0 + notch);
        }

        notesHaveChanged = true;
    }
    else if (focusedFieldName == "note1")
    {
        setNote1(note1 + notch);
        notesHaveChanged = true;
    }

    return timesHaveChanged || notesHaveChanged;
}

void WithTimesAndNotes::setNote0(const NoteNumber noteNumber)
{
    note0 = noteNumber;

    if (note0 > note1)
    {
        note1 = note0;
    }

    displayNotes();
}

void WithTimesAndNotes::setNote1(const NoteNumber noteNumber)
{
    note1 = noteNumber;

    if (note1 < note0)
    {
        note0 = note1;
    }

    displayNotes();
}

void WithTimesAndNotes::setTime0(const int newTime0)
{
    time0 = newTime0;

    if (newTime0 > time1)
    {
        time1 = newTime0;
    }

    displayTime();
}

void WithTimesAndNotes::setTime1(const int newTime1)
{
    time1 = newTime1;

    if (newTime1 < time0)
    {
        time0 = newTime1;
    }

    displayTime();
}
