#include "file/mid/MidiReader.hpp"

#include "midi/MidiFile.hpp"
#include "midi/MidiTrack.hpp"
#include "midi/event/ChannelAftertouch.hpp"
#include "midi/event/Controller.hpp"
#include "midi/event/NoteAftertouch.hpp"
#include "midi/event/NoteOff.hpp"
#include "midi/event/NoteOn.hpp"
#include "midi/event/PitchBend.hpp"
#include "midi/event/ProgramChange.hpp"
#include "midi/event/SystemExclusive.hpp"
#include "midi/event/meta/Tempo.hpp"
#include "midi/event/meta/Text.hpp"
#include "midi/event/meta/TimeSignatureEvent.hpp"
#include "midi/event/meta/TrackName.hpp"
#include <Mpc.hpp>
#include "sequencer/ChannelPressureEvent.hpp"
#include "sequencer/ControlChangeEvent.hpp"
#include "sequencer/MixerEvent.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/NoteEvent.hpp"
#include "sequencer/PitchBendEvent.hpp"
#include "sequencer/PolyPressureEvent.hpp"
#include "sequencer/ProgramChangeEvent.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SystemExclusiveEvent.hpp"
#include "sequencer/TempoChangeEvent.hpp"

using namespace mpc::file::mid;
using namespace mpc::midi::event;
using namespace mpc::sequencer;

MidiReader::MidiReader(std::shared_ptr<std::istream> istream, std::weak_ptr<Sequence> _dest)
        : dest (_dest)
{
    midiFile = std::make_unique<mpc::midi::MidiFile>(istream);
}

void MidiReader::parseSequence(mpc::Mpc& mpc)
{
    auto lSequencer = mpc.getSequencer();
    auto midiTracks = midiFile->getTracks();
    auto lengthInTicks = (int)(midiFile->getLengthInTicks() + midiTracks[0].lock()->getEndOfTrackDelta());

    std::vector<std::weak_ptr<meta::TimeSignature>> timeSignatures;
    std::vector<std::weak_ptr<meta::Tempo>> tempoChanges;

    auto firstLoopBar = -1;
    auto lastLoopBar = -1;

    auto sequence = dest.lock();
    sequence->setUsed(true);

    bool isMpc2000XlMidiFile = false;

    for (auto& event : midiTracks[0].lock()->getEvents())
    {
        if (const auto textEvent = std::dynamic_pointer_cast<meta::Text>(event.lock()); textEvent)
        {
            auto text = textEvent->getText();

            if (text.find("LOOP=ON") != std::string::npos)
                sequence->setLoopEnabled(true);
            else if (text.find("LOOP=OFF") != std::string::npos)
                sequence->setLoopEnabled(false);

            if (text.find("TEMPO=ON") != std::string::npos)
                lSequencer->setTempoSourceSequence(true);
            else if (text.find("TEMPO=OFF") != std::string::npos)
                lSequencer->setTempoSourceSequence(false);

            firstLoopBar = stoi(text.substr(15, 18));
            lastLoopBar = -1;

            if (isInteger(text.substr(23, 26)))
                lastLoopBar = stoi(text.substr(23, 26));
        }
        else if (const auto trackNameEvent = std::dynamic_pointer_cast<meta::TrackName>(event.lock());
                 trackNameEvent && trackNameEvent->getTrackName().find("MPC2000XL 1.00  ") != std::string::npos)
        {
            isMpc2000XlMidiFile = true;
            auto sequenceName = trackNameEvent->getTrackName().substr(16);
            sequence->setName(sequenceName);
        }
        else if (const auto tempoEvent = std::dynamic_pointer_cast<meta::Tempo>(event.lock()); tempoEvent)
        {
            tempoChanges.push_back(tempoEvent);
        }
        else if (const auto timeSigEvent = std::dynamic_pointer_cast<meta::TimeSignature>(event.lock()); timeSigEvent)
        {
            timeSignatures.push_back(timeSigEvent);
        }
    }

    const auto initialTempo = tempoChanges.empty() ? 120 : tempoChanges[0].lock()->getBpm();

    sequence->setInitialTempo(initialTempo);

    if (!lSequencer->isTempoSourceSequenceEnabled())
    {
        lSequencer->setTempo(initialTempo);
    }

    for (int i = 1; i < tempoChanges.size(); i++)
    {
        auto lTcMidi = tempoChanges[i].lock();
        auto tce = sequence->addTempoChangeEvent(lTcMidi->getTick());
        auto ratio = (float)(lTcMidi->getBpm()) / initialTempo;
        tce->setRatio((int)(ratio * 1000.0));
    }

    if (timeSignatures.size() == 1)
        lengthInTicks = midiTracks[0].lock()->getEndOfTrackDelta();

    int accumLength = 0;
    int barCounter = 0;

    for (int i = 0; i < timeSignatures.size(); i++)
    {
        auto current = timeSignatures[i].lock();
        std::shared_ptr<meta::TimeSignature> next;

        if (timeSignatures.size() > i + 1)
            next = timeSignatures[i + 1].lock();

        if (next)
        {
            while (accumLength < next->getTick())
            {
                sequence->setTimeSignature(barCounter, current->getNumerator(), current->getRealDenominator());
                int barLength = sequence->getBarLengthsInTicks()[barCounter++];
                accumLength += barLength;
            }
        }
        else
        {
            while (accumLength < lengthInTicks)
            {
                sequence->setTimeSignature(barCounter, current->getNumerator(), current->getRealDenominator());
                int barLength = sequence->getBarLengthsInTicks()[barCounter++];
                accumLength += barLength;
            }
        }
    }

    sequence->setLastBarIndex(barCounter - 1);
    sequence->setFirstLoopBarIndex(firstLoopBar);

    if (lastLoopBar == -1)
    {
        sequence->setLastLoopBarIndex(sequence->getLastBarIndex());
        sequence->setLastLoopBarIndex(sequence->getLastLoopBarIndex() + 1);
    }
    else
    {
        sequence->setLastLoopBarIndex(lastLoopBar);
    }

    std::unique_ptr<NoteOnEvent> nVariation;
    const int maxNoteOffTick = 999999999;

    const std::string trackDataPrefix = "TRACK DATA:";

    const int midiTrackStartIndexForCollectingNoteEvents = isMpc2000XlMidiFile ? 1 : 0;

    for (int i = midiTrackStartIndexForCollectingNoteEvents; i < midiTracks.size(); i++)
    {
        auto mt = midiTracks[i].lock();
        auto trackIndex = isMpc2000XlMidiFile ? i - 1 : i;
        auto deviceIndex = 0;

        for (auto& e: mt->getEvents())
        {
            auto textEvent = std::dynamic_pointer_cast<meta::Text>(e.lock());
            if (textEvent) {
                auto text = textEvent->getText();
                if (text.find(trackDataPrefix) != std::string::npos)
                {
                    auto payload = text.substr(trackDataPrefix.length());
                    trackIndex = stoi(payload.substr(0, 2));
                    auto deviceIndexStr = payload.substr(2, 2);

                    if (deviceIndexStr != "C0")
                    {
                        deviceIndex = stoi(deviceIndexStr, 0, 16) - stoi(std::string("E0"), 0, 16);
                    }
                    break;
                }
            }
        }
        std::vector<std::shared_ptr<ChannelEvent>> noteOffs;
        std::vector<std::shared_ptr<NoteOnEvent>> noteOns;

        auto track = sequence->purgeTrack(trackIndex);
        track->setDeviceIndex(deviceIndex);
        track->setUsed(true);

        if (isMpc2000XlMidiFile)
        {
            for (auto& me : mt->getEvents())
            {
                auto noteOff = std::dynamic_pointer_cast<NoteOff>(me.lock());
                auto noteOn = std::dynamic_pointer_cast<NoteOn>(me.lock());

                if (noteOff)
                {
                    nVariation = std::make_unique<NoteOnEvent>();
                    nVariation->incrementVariationType(noteOff->getNoteValue());
                    nVariation->setVariationValue(noteOff->getVelocity());
                }
                else if (noteOn)
                {
                    if (noteOn->getVelocity() == 0)
                    {
                        if (getNumberOfNotes(noteOn->getNoteValue(), noteOns) > getNumberOfNoteOns(noteOn->getNoteValue(), noteOffs))
                        {
                            noteOffs.push_back(noteOn);
                        }
                    }
                    else
                    {
                        if (getNumberOfNotes(noteOn->getNoteValue(), noteOns) > getNumberOfNoteOns(noteOn->getNoteValue(), noteOffs))
                        {
                            noteOffs.emplace_back(std::make_shared<NoteOn>(noteOn->getTick(), 0, (noteOn->getNoteValue()), 0));
                        }

                        auto ne = std::make_shared<NoteOnEvent>(noteOn->getNoteValue());
                        ne->setTick(noteOn->getTick());
                        ne->setVelocity(noteOn->getVelocity());

                        if (nVariation)
                        {
                            ne->incrementVariationType(nVariation->getVariationType());
                            ne->setVariationValue(nVariation->getVariationValue());
                        }
                        else
                        {
                            ne->setVariationValue(64);
                        }

                        noteOns.emplace_back(ne);
                    }
                }
            }
        }
        else // if !isMpc2000XlMidiFile
        {
            for (auto &me: mt->getEvents())
            {
                auto noteOff = std::dynamic_pointer_cast<NoteOff>(me.lock());
                auto noteOn = std::dynamic_pointer_cast<NoteOn>(me.lock());

                if (noteOff)
                {
                    noteOffs.push_back(noteOff);
                }
                else if (noteOn)
                {
                    auto mpcNoteEvent = std::make_shared<NoteOnEvent>(noteOn->getNoteValue());
                    mpcNoteEvent->setTick(noteOn->getTick());
                    mpcNoteEvent->setVelocity(noteOn->getVelocity());
                    noteOns.emplace_back(mpcNoteEvent);
                }
            }
        }

        for (auto& n: noteOns)
        {
            auto noteOn = track->recordNoteEventSynced(n->getTick(), n->getNote(), n->getVelocity());

            int indexCandidate = -1;

            for (int k = 0; k < noteOffs.size(); k++)
            {
                int noteValue;
                int tick;

                auto noteOff1 = std::dynamic_pointer_cast<NoteOn>(noteOffs[k]);

                if (noteOff1)
                {
                    // isMpc2000XlMidiFile == true. MPC2000XL MIDI files use MIDI Note On events
                    // to indicate the end of a note.
                    noteValue = noteOff1->getNoteValue();
                    tick = noteOff1->getTick();
                }
                else
                {
                    // Ordinary MIDI file. The end of a note is indicated by a note off event.
                    auto noteOff2 = std::dynamic_pointer_cast<NoteOff>(noteOffs[k]);
                    noteValue = noteOff2->getNoteValue();
                    tick = noteOff2->getTick();
                }

                if (noteValue == noteOn->getNote() && tick >= noteOn->getTick())
                {
                    if (tick < maxNoteOffTick)
                    {
                        indexCandidate = k;
                        break;
                    }
                }
            }

            if (indexCandidate != -1)
            {
                noteOn->setDuration((int)(noteOffs[indexCandidate]->getTick() - noteOn->getTick()));
                noteOffs.erase(noteOffs.begin() + indexCandidate);
            }
            else
            {
                noteOn->setDuration(24);
            }
        }

        for (auto& me : mt->getEvents())
        {
            if (const auto sysEx = std::dynamic_pointer_cast<mpc::midi::event::SystemExclusiveEvent>(me.lock()); sysEx)
            {
                auto sysExEventBytes = sysEx->getData();

                if (sysExEventBytes.size() == 8 && sysExEventBytes[0] == 71 && sysExEventBytes[1] == 0 && sysExEventBytes[2] == 68 && sysExEventBytes[3] == 69 && sysExEventBytes[7] == (char) 247)
                {
                    auto mixerEvent = std::make_shared<MixerEvent>();
                    track->addEvent(sysEx->getTick(), mixerEvent);
                    mixerEvent->setParameter(sysExEventBytes[4] - 1);
                    mixerEvent->setPadNumber(sysExEventBytes[5]);
                    mixerEvent->setValue(sysExEventBytes[6]);
                }
                else
                {
                    sysExEventBytes = std::vector<char>(sysEx->getData().size() + 1);
                    sysExEventBytes[0] = 0xF0;

                    for (int j = 0; j < sysEx->getData().size(); j++)
                    {
                        sysExEventBytes[j + 1] = sysEx->getData()[j];
                    }

                    auto see = std::make_shared<mpc::sequencer::SystemExclusiveEvent>();
                    track->addEvent(sysEx->getTick(), see);
                    std::vector<unsigned char> tmp;

                    for (char c : sysExEventBytes)
                    {
                        tmp.push_back(static_cast<unsigned char>(c));
                    }

                    see->setBytes(tmp);
                }
            }
            else if (const auto noteAfterTouch = std::dynamic_pointer_cast<NoteAftertouch>(me.lock()); noteAfterTouch)
            {
                auto ppe = std::make_shared<PolyPressureEvent>();
                track->addEvent(noteAfterTouch->getTick(), ppe);
                ppe->setNote(noteAfterTouch->getNoteValue());
                ppe->setAmount(noteAfterTouch->getAmount());
            }
            else if (const auto channelAfterTouch = std::dynamic_pointer_cast<ChannelAftertouch>(me.lock()); channelAfterTouch)
            {
                auto cpe = std::make_shared<ChannelPressureEvent>();
                track->addEvent(channelAfterTouch->getTick(), cpe);
                cpe->setAmount(channelAfterTouch->getAmount());
            }
            else if (const auto programChange = std::dynamic_pointer_cast<ProgramChange>(me.lock()); programChange)
            {
                auto pce = std::make_shared<ProgramChangeEvent>();
                track->addEvent(programChange->getTick(), pce);
                pce->setProgram(programChange->getProgramNumber() + 1);
            }
            else if (const auto trackName = std::dynamic_pointer_cast<meta::TrackName>(me.lock()); trackName && !trackName->getTrackName().empty())
            {
                track->setName(trackName->getTrackName());
            }
            else if (const auto controller = std::dynamic_pointer_cast<Controller>(me.lock()); controller)
            {
                auto cce = std::make_shared<ControlChangeEvent>();
                track->addEvent(controller->getTick(), cce);
                cce->setController(controller->getControllerType());
                cce->setAmount(controller->getValue());
            }
            else if (const auto pitchBend = std::dynamic_pointer_cast<PitchBend>(me.lock()); pitchBend)
            {
                auto pbe = std::make_shared<PitchBendEvent>();
                track->addEvent(pitchBend->getTick(), pbe);
                pbe->setAmount(pitchBend->getBendAmount() - 8192);
            }
        }
    }
}

bool MidiReader::isInteger(std::string s)
{
    for (auto& c : s)
    {
        if (!std::isdigit(c))
        {
            return false;
        }
    }

    return true;
}

int MidiReader::getNumberOfNoteOns(int noteValue, std::vector<std::shared_ptr<ChannelEvent>> allNotes)
{
    int counter = 0;

    for (auto& no : allNotes)
    {
        auto  noteOn = std::dynamic_pointer_cast<NoteOn>(no);

        if (noteOn)
        {
            if (noteOn->getNoteValue() == noteValue) counter++;
        }
    }

    return counter;
}

int MidiReader::getNumberOfNotes(int noteValue, std::vector<std::shared_ptr<NoteOnEvent>> allNotes)
{
    int counter = 0;

    for (auto& ne : allNotes)
        if (ne->getNote() == noteValue) counter++;

    return counter;
}
