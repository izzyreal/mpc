#include "AllNoteEvent.hpp"

#include "AllEvent.hpp"
#include "AllSequence.hpp"

#include <sequencer/Event.hpp>
#include <sequencer/NoteEvent.hpp>

#include <file/BitUtil.hpp>

using namespace mpc::file::all;
using namespace mpc::sequencer;
using namespace moduru::file;
using namespace std;

shared_ptr<NoteEvent> AllNoteEvent::bytesToMpcEvent(const vector<char>& bytes)
{
    auto event = make_shared<NoteEvent>();
    
    event->setNote(bytes[NOTE_NUMBER_OFFSET]);
    event->setTick(AllEvent::readTick(bytes));
    event->setTrack(readTrackNumber(bytes));
    event->setDuration(readDuration(bytes));
    event->setVelocity(readVelocity(bytes));
    event->setVariationTypeNumber(readVariationType(bytes));
    event->setVariationValue(readVariationValue(bytes));
    
    return event;
}

vector<char> AllNoteEvent::mpcEventToBytes(std::shared_ptr<NoteEvent> event)
{
    vector<char> bytes(AllSequence::EVENT_SEG_LENGTH);
	
	bytes[NOTE_NUMBER_OFFSET] = static_cast<int8_t>(event->getNote());
	
    try {
		writeVelocity(bytes, event->getVelocity());
		writeTrackNumber(bytes, event->getTrack());
		writeVariationType(bytes, event->getVariationType());
		writeVariationValue(bytes, event->getVariationValue());
		AllEvent::writeTick(bytes, static_cast<int>(event->getTick()));
		writeDuration(bytes, static_cast<int>(event->getDuration()));
	}
	catch (exception e) {
		throw e;
	}
    
    return bytes;
}

vector<int> AllNoteEvent::DURATION_BYTE1_BIT_RANGE = vector<int>{ 4, 7 };
vector<int> AllNoteEvent::DURATION_BYTE2_BIT_RANGE = vector<int>{ 6, 7 };
vector<int> AllNoteEvent::TRACK_NUMBER_BIT_RANGE = vector<int>{ 0, 5 };
vector<int> AllNoteEvent::VELOCITY_BIT_RANGE = vector<int>{ 0, 6 };
vector<int> AllNoteEvent::VAR_VALUE_BIT_RANGE = vector<int>{ 0, 6 };

int AllNoteEvent::readDuration(const vector<char>& bytes)
{
    auto b1 = bytes[DURATION_BYTE1_OFFSET];
    auto b2 = bytes[DURATION_BYTE2_OFFSET];
    auto b3 = bytes[DURATION_BYTE3_OFFSET];
    
    if (static_cast<unsigned char>(b1) == 255 && static_cast<unsigned char>(b2) == 255 && static_cast<unsigned char>(b3) == 255)
    {
        return -1;
    }

    b1 = BitUtil::removeUnusedBits(b1, DURATION_BYTE1_BIT_RANGE);
    b2 = BitUtil::removeUnusedBits(b2, DURATION_BYTE2_BIT_RANGE);
    
    auto i1 = static_cast<int>(b1 & 255);
    auto i2 = static_cast<int>(b2 & 255);
    auto i3 = static_cast<int>(b3 & 255);
    
    return (i1 << 6) + (i2 << 2) + i3;
}

int AllNoteEvent::readTrackNumber(const vector<char>& bytes)
{
    auto b = BitUtil::removeUnusedBits(bytes[TRACK_NUMBER_OFFSET], TRACK_NUMBER_BIT_RANGE);
    return b;
}

int AllNoteEvent::readVelocity(const vector<char>& bytes)
{
    auto b = BitUtil::removeUnusedBits(bytes[VELOCITY_OFFSET], VELOCITY_BIT_RANGE);
    return b;
}

int AllNoteEvent::readVariationValue(const vector<char>& bytes)
{
    auto b = BitUtil::removeUnusedBits(bytes[VAR_VALUE_OFFSET], VAR_VALUE_BIT_RANGE);
    return b;
}

int AllNoteEvent::readVariationType(const vector<char>& bytes)
{
    auto byte1 = bytes[VAR_TYPE_BYTE1_OFFSET];
    auto byte2 = bytes[VAR_TYPE_BYTE2_OFFSET];
    
    auto b1 = BitUtil::isBitOn(byte1, VAR_TYPE_BYTE1_BIT);
    auto b2 = BitUtil::isBitOn(byte2, VAR_TYPE_BYTE2_BIT);
 
    if (b1 && b2)
        return 3;

    if (b1 && !b2)
        return 2;

    if (!b1 && b2)
        return 1;

    if (!b1 && !b2)
        return 0;

    return -1;
}

void AllNoteEvent::writeVelocity(vector<char>& event, int v)
{
    auto value = static_cast< int8_t >(v);
    event[VELOCITY_OFFSET] = BitUtil::stitchBytes(event[VELOCITY_OFFSET], vector<int>{ VAR_TYPE_BYTE1_BIT, VAR_TYPE_BYTE1_BIT }, value, VELOCITY_BIT_RANGE);
    return event;
}

void AllNoteEvent::writeTrackNumber(vector<char>& event, int t)
{
    auto value = static_cast< int8_t >(t);
    event[TRACK_NUMBER_OFFSET] = BitUtil::stitchBytes(event[TRACK_NUMBER_OFFSET], DURATION_BYTE2_BIT_RANGE, value, TRACK_NUMBER_BIT_RANGE);
    return event;
}

void AllNoteEvent::writeVariationValue(vector<char>& event, int v)
{
    auto value = static_cast< int8_t >(v);
    event[VAR_VALUE_OFFSET] = BitUtil::stitchBytes(event[VAR_VALUE_OFFSET], vector<int>{ VAR_TYPE_BYTE2_BIT, VAR_TYPE_BYTE2_BIT }, value, VAR_VALUE_BIT_RANGE);
    return event;
}

void AllNoteEvent::writeDuration(vector<char>& event, int duration)
{
    auto s1 = static_cast< short >(duration >> 6);
    auto s2 = static_cast< short >(duration >> 2);
    auto s3 = static_cast< short >(duration & 255);
    
    event[DURATION_BYTE1_OFFSET] = BitUtil::stitchBytes(event[DURATION_BYTE1_OFFSET], AllEvent::TICK_BYTE3_BIT_RANGE, static_cast< int8_t >(s1), DURATION_BYTE1_BIT_RANGE);
    
    event[DURATION_BYTE2_OFFSET] = BitUtil::stitchBytes(event[DURATION_BYTE2_OFFSET], TRACK_NUMBER_BIT_RANGE, static_cast< int8_t >(s2), DURATION_BYTE2_BIT_RANGE);

    event[DURATION_BYTE3_OFFSET] = static_cast< int8_t >(s3);

    return event;
}

void AllNoteEvent::writeVariationType(vector<char>& event, int type)
{
    auto byte1 = event[VAR_TYPE_BYTE1_OFFSET];
    auto byte2 = event[VAR_TYPE_BYTE2_OFFSET];
    switch (type) {
    case 0:
        BitUtil::setBit(byte1, VAR_TYPE_BYTE1_BIT, false);
        BitUtil::setBit(byte2, VAR_TYPE_BYTE2_BIT, false);
        break;
    case 1:
        BitUtil::setBit(byte1, VAR_TYPE_BYTE1_BIT, false);
        BitUtil::setBit(byte2, VAR_TYPE_BYTE2_BIT, true);
        break;
    case 2:
        BitUtil::setBit(byte1, VAR_TYPE_BYTE1_BIT, true);
        BitUtil::setBit(byte2, VAR_TYPE_BYTE2_BIT, false);
        break;
    case 3:
        BitUtil::setBit(byte1, VAR_TYPE_BYTE1_BIT, true);
        BitUtil::setBit(byte2, VAR_TYPE_BYTE2_BIT, true);
        break;
    }

    event[VAR_TYPE_BYTE1_OFFSET] = byte1;
    event[VAR_TYPE_BYTE2_OFFSET] = byte2;
    return event;
}
