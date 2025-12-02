#include "EraseScreen.hpp"

#include "sampler/Sampler.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/NoteOnEvent.hpp"
#include "sequencer/SeqUtil.hpp"

#include "Util.hpp"

#include "StrUtil.hpp"
#include "lcdgui/Label.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;

EraseScreen::EraseScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "erase", layerIndex)
{
}

void EraseScreen::open()
{
    setTrack(sequencer.lock()->getSelectedTrackIndex());

    findField("note1")->setLocation(116, 42);

    setTime0(0);

    const auto seq = sequencer.lock()->getSelectedSequence();
    setTime1(seq->getLastTick());

    displayErase();
    displayNotes();
    displayTime();
    displayTrack();
    displayType();
}

void EraseScreen::turnWheel(const int i)
{
    if (checkAllTimesAndNotes(mpc, i))
    {
        return;
    }

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "track")
    {
        setTrack(track + i);
    }
    else if (focusedFieldName == "erase")
    {
        setErase(erase + i);
    }
    else if (focusedFieldName == "type")
    {
        setType(type + i);
    }
}

void EraseScreen::function(const int i)
{
    switch (i)
    {
        case 3:
            openScreenById(ScreenId::SequencerScreen);
            break;
        case 4:
            doErase();
            openScreenById(ScreenId::SequencerScreen);
            break;
        default:;
    }
}

void EraseScreen::displayTrack() const
{
    std::string trackName;

    if (track == -1)
    {
        trackName = "ALL";
    }
    else
    {
        const auto sequence = sequencer.lock()->getSelectedSequence();
        trackName = sequence->getTrack(track)->getActualName();
    }

    findField("track")->setTextPadded(track + 1, " ");
    findLabel("track-name")->setText("-" + trackName);
}

void EraseScreen::displayTime()
{
    const auto sequence = sequencer.lock()->getSelectedSequence().get();
    findField("time0")->setTextPadded(
        SeqUtil::getBarFromTick(sequence, time0) + 1, "0");
    findField("time1")->setTextPadded(SeqUtil::getBeat(sequence, time0) + 1,
                                      "0");
    findField("time2")->setTextPadded(SeqUtil::getClock(sequence, time0), "0");
    findField("time3")->setTextPadded(
        SeqUtil::getBarFromTick(sequence, time1) + 1, "0");
    findField("time4")->setTextPadded(SeqUtil::getBeat(sequence, time1) + 1,
                                      "0");
    findField("time5")->setTextPadded(SeqUtil::getClock(sequence, time1), "0");
}

void EraseScreen::displayErase() const
{
    findField("erase")->setText(eraseNames[erase]);
}

void EraseScreen::displayType() const
{
    findField("type")->Hide(erase == 0);

    if (erase > 0)
    {
        findField("type")->setText(typeNames[type]);
    }
}

void EraseScreen::displayNotes()
{
    if (const auto busType = track >= 0 ? sequencer.lock()
                                              ->getSelectedSequence()
                                              ->getTrack(track)
                                              ->getBusType()
                                        : BusType::DRUM1;
        isMidiBusType(busType))
    {
        findField("note0")->setAlignment(Alignment::Centered, 18);
        findField("note1")->setAlignment(Alignment::Centered, 18);
        findField("note0")->setLocation(62, 42);
    }
    else
    {
        findField("note0")->setAlignment(Alignment::None);
        findField("note1")->setAlignment(Alignment::None);
        findField("note0")->setLocation(61, 42);
    }

    if (erase != 0 && ((erase == 1 && type != 0) || (erase == 2 && type != 0)))
    {
        findField("note0")->Hide(true);
        findLabel("note0")->Hide(true);
        findField("note1")->Hide(true);
        findLabel("note1")->Hide(true);
        return;
    }

    const auto busType = track >= 0 ? sequencer.lock()
                                          ->getSelectedSequence()
                                          ->getTrack(track)
                                          ->getBusType()
                                    : BusType::DRUM1;

    findField("note0")->Hide(false);
    findLabel("note0")->Hide(false);
    findField("note1")->Hide(isDrumBusType(busType));
    findLabel("note1")->Hide(isDrumBusType(busType));

    if (isMidiBusType(busType))
    {
        findField("note0")->setSize(47, 9);
        findField("note0")->setText(
            StrUtil::padLeft(std::to_string(midiNote0), " ", 3) + "(" +
            Util::noteNames()[midiNote0] + ")");
        findField("note1")->setText(
            StrUtil::padLeft(std::to_string(midiNote1), " ", 3) + "(" +
            Util::noteNames()[midiNote1] + ")");
    }
    else
    {
        findField("note0")->setSize(37, 9);

        if (drumNoteNumber == AllDrumNotes)
        {
            findField("note0")->setText("ALL");
        }
        else
        {
            const auto program = getProgramOrThrow();
            const auto padIndexWithBank =
                program->getPadIndexFromNote(DrumNoteNumber(drumNoteNumber));
            const auto padName = sampler.lock()->getPadName(padIndexWithBank);
            findField("note0")->setText(std::to_string(drumNoteNumber) + "/" +
                                        padName);
        }
    }
}

void EraseScreen::setTrack(const int i)
{
    track = std::clamp(i, static_cast<int>(AllTrackIndex),
                       static_cast<int>(Mpc2000XlSpecs::LAST_TRACK_INDEX));
    displayTrack();
    displayNotes();
}

void EraseScreen::setErase(const int i)
{
    erase = std::clamp(i, 0, 2);
    displayErase();
    displayType();
    displayNotes();
}

void EraseScreen::setType(const int i)
{
    type = std::clamp(i, 0, 6);
    displayType();
    displayNotes();
}

void EraseScreen::doErase() const
{
    const auto firstTrackIndex = track < 0 ? 0 : track;
    const auto lastTrackIndex = track < 0 ? 63 : track;

    const auto midi = track >= 0 && isMidiBusType(sequencer.lock()
                                                      ->getSelectedSequence()
                                                      ->getTrack(track)
                                                      ->getBusType());

    const auto noteA = midi ? midiNote0 : drumNoteNumber;
    const auto noteB = midi ? midiNote1 : -1;

    const auto seq = sequencer.lock()->getSelectedSequence();

    const auto selectedType = eventTypes[type];

    for (auto trackIndex = firstTrackIndex; trackIndex <= lastTrackIndex;
         trackIndex++)
    {
        const auto seqTrack = seq->getTrack(trackIndex);

        for (auto eventIndex =
                 static_cast<int>(seqTrack->getEvents().size()) - 1;
             eventIndex >= 0; eventIndex--)
        {
            const auto event = seqTrack->getEvent(eventIndex);
            const auto noteEvent =
                std::dynamic_pointer_cast<NoteOnEvent>(event);

            if (event->getTick() >= time0 && event->getTick() < time1)
            {
                if (erase == 0 ||
                    (erase == 1 && event->getTypeName() != selectedType) ||
                    erase == 2 && event->getTypeName() != selectedType)
                {
                    if (noteEvent)
                    {
                        if (const auto noteNumber = noteEvent->getNote();
                            (midi && noteNumber >= noteA &&
                             noteNumber <= noteB) ||
                            (!midi &&
                             (noteA == AllDrumNotes || noteA == noteNumber)))
                        {
                            seqTrack->removeEvent(noteEvent);
                        }
                    }
                    else
                    {
                        seqTrack->removeEvent(noteEvent);
                    }
                }
            }
        }
    }
}
