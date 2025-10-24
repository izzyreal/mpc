#include "midi/event/meta/MetaEvent.hpp"

#include "midi/event/meta/CopyrightNotice.hpp"
#include "midi/event/meta/CuePoint.hpp"
#include "midi/event/meta/EndOfTrack.hpp"
#include "midi/event/meta/GenericMetaEvent.hpp"
#include "midi/event/meta/InstrumentName.hpp"
#include "midi/event/meta/KeySignature.hpp"
#include "midi/event/meta/Marker.hpp"
#include "midi/event/meta/MetaEventData.hpp"
#include "midi/event/meta/MidiChannelPrefix.hpp"
#include "midi/event/meta/SequenceNumber.hpp"
#include "midi/event/meta/SequencerSpecificEvent.hpp"
#include "midi/event/meta/SmpteOffset.hpp"
#include "midi/event/meta/Tempo.hpp"
#include "midi/event/meta/Text.hpp"
#include "midi/event/meta/TimeSignatureEvent.hpp"
#include "midi/event/meta/TrackName.hpp"
#include "midi/util/VariableLengthInt.hpp"

using namespace mpc::midi::event::meta;

MetaEvent::MetaEvent(int tick, int delta, int type)
    : mpc::midi::event::MidiEvent(tick, delta)
{
    mType = type & 255;
}

void MetaEvent::writeToOutputStream(std::ostream &out, bool writeType)
{
    writeToOutputStream(out);
}

void MetaEvent::writeToOutputStream(std::ostream &out)
{
    MidiEvent::writeToOutputStream(out, true);
    out << (char)0xFF;
    out << (char)mType;
}

std::shared_ptr<MetaEvent> MetaEvent::parseMetaEvent(int tick, int delta, std::stringstream &in)
{
    auto eventData = MetaEventData(in);
    auto isText = false;
    switch (eventData.type)
    {
    case SEQUENCE_NUMBER:
    case MIDI_CHANNEL_PREFIX:
    case END_OF_TRACK:
    case TEMPO:
    case SMPTE_OFFSET:
    case TIME_SIGNATURE:
    case KEY_SIGNATURE:
        break;
    case TEXT_EVENT:
    case COPYRIGHT_NOTICE:
    case TRACK_NAME:
    case INSTRUMENT_NAME:
    case LYRICS:
    case MARKER:
    case CUE_POINT:
    case SEQUENCER_SPECIFIC:
    default:
        isText = true;
        break;
    }

    if (isText)
    {
        std::string text;
        for (char c : eventData.data)
        {
            text.push_back(c);
        }
        switch (eventData.type)
        {
        case TEXT_EVENT:
            return std::make_shared<Text>(tick, delta, text);
        case COPYRIGHT_NOTICE:
            return std::make_shared<CopyrightNotice>(tick, delta, text);
        case TRACK_NAME:
            return std::make_shared<TrackName>(tick, delta, text);
        case INSTRUMENT_NAME:
            return std::make_shared<InstrumentName>(tick, delta, text);
        case LYRICS:
            // return new Lyrics(tick, delta, text);
            return nullptr;
        case MARKER:
            return std::make_shared<Marker>(tick, delta, text);
        case CUE_POINT:
            return std::make_shared<CuePoint>(tick, delta, text);
        case SEQUENCER_SPECIFIC:
            return std::make_shared<SequencerSpecificEvent>(tick, delta, eventData.data);
        default:
            return std::make_shared<GenericMetaEvent>(tick, delta, &eventData);
        }
    }

    switch (eventData.type)
    {
    case SEQUENCE_NUMBER:
        return SequenceNumber::parseSequenceNumber(tick, delta, &eventData);
    case MIDI_CHANNEL_PREFIX:
        return MidiChannelPrefix::parseMidiChannelPrefix(tick, delta, &eventData);
    case END_OF_TRACK:
        return std::make_shared<EndOfTrack>(tick, delta);
    case TEMPO:
        return Tempo::parseTempo(tick, delta, &eventData);
    case SMPTE_OFFSET:
        return SmpteOffset::parseSmpteOffset(tick, delta, &eventData);
    case TIME_SIGNATURE:
        return TimeSignature::parseTimeSignature(tick, delta, &eventData);
    case KEY_SIGNATURE:
        return KeySignature::parseKeySignature(tick, delta, &eventData);
    }

    std::string error = "Completely broken in MetaEvent.parseMetaEvent()";
    return nullptr;
}

const int MetaEvent::SEQUENCE_NUMBER;
const int MetaEvent::TEXT_EVENT;
const int MetaEvent::COPYRIGHT_NOTICE;
const int MetaEvent::TRACK_NAME;
const int MetaEvent::INSTRUMENT_NAME;
const int MetaEvent::LYRICS;
const int MetaEvent::MARKER;
const int MetaEvent::CUE_POINT;
const int MetaEvent::MIDI_CHANNEL_PREFIX;
const int MetaEvent::END_OF_TRACK;
const int MetaEvent::TEMPO;
const int MetaEvent::SMPTE_OFFSET;
const int MetaEvent::TIME_SIGNATURE;
const int MetaEvent::KEY_SIGNATURE;
const int MetaEvent::SEQUENCER_SPECIFIC;
