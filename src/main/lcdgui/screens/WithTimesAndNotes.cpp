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
                                      const Sequence *sequence)
{
    const auto sequenceToUse =
        sequence != nullptr ? sequence
                            : mpc.getSequencer()->getSelectedSequence().get();

    const auto focusedFieldName = mpc.getLayeredScreen()
                                      ->getCurrentScreen()
                                      ->getFocusedFieldNameOrThrow();

    if (focusedFieldName == "time0")
    {
        setTime0(SeqUtil::setBar(SeqUtil::getBar(sequenceToUse, time0) + notch,
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
        setTime1(SeqUtil::setBar(SeqUtil::getBar(sequenceToUse, time1) + notch,
                                 sequenceToUse, time1));
        return true;
    }
    if (focusedFieldName == "time4")
    {
        const auto currentBeat = SeqUtil::getBeat(sequenceToUse, time1);
        const auto newBeat = currentBeat + notch;
        const auto newPos = SeqUtil::setBeat(newBeat, sequenceToUse, time1);
        setTime1(newPos);
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
                                              const Sequence *sequence,
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
                                 : mpc.getSequencer()->getSelectedTrack().get();
            isMidiBusType(trackToUse->getBusType()))
        {
            setMidiNote0(NoteNumber(midiNote0 + notch));
        }
        else
        {
            setDrumNote(DrumNoteNumber(drumNoteNumber + notch));
            displayDrumNotes();
        }

        notesHaveChanged = true;
    }
    else if (focusedFieldName == "note1")
    {
        setMidiNote1(NoteNumber(midiNote1 + notch));
        notesHaveChanged = true;
    }

    return timesHaveChanged || notesHaveChanged;
}

void WithTimesAndNotes::setDrumNote(const DrumNoteNumber drumNoteNumberToUse)
{
    drumNoteNumber = drumNoteNumberToUse;
    displayDrumNotes();
}

void WithTimesAndNotes::setMidiNote0(const NoteNumber noteNumber)
{
    midiNote0 = std::clamp(noteNumber, MinNoteNumber, MaxNoteNumber);

    if (midiNote0 > midiNote1)
    {
        midiNote1 = midiNote0;
    }

    displayNotes();
}

void WithTimesAndNotes::setMidiNote1(const NoteNumber noteNumber)
{
    midiNote1 = std::clamp(noteNumber, MinNoteNumber, MaxNoteNumber);

    if (midiNote1 < midiNote0)
    {
        midiNote0 = midiNote1;
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
