#include "AllNoteEvent.hpp"

#include "AllEvent.hpp"
#include "AllSequence.hpp"

#include <sequencer/NoteEvent.hpp>

#include <file/BitUtil.hpp>

using namespace mpc::file::all;
using namespace mpc::sequencer;
using namespace moduru::file;

std::shared_ptr<NoteOnEvent> AllNoteOnEvent::bytesToMpcEvent(const std::vector<char>& bytes)
{
    auto event = std::make_shared<NoteOnEvent>();
    
    event->setNote(bytes[NOTE_NUMBER_OFFSET]);
    event->setTick(AllEvent::readTick(bytes));
    auto track = readTrackNumber(bytes);
    event->setTrack(track);
    event->setDuration(readDuration(bytes)- (track * 4));
    event->setVelocity(readVelocity(bytes));
    event->setVariationType(readVariationType(bytes));
    event->setVariationValue(readVariationValue(bytes));
    
    return event;
}

std::vector<char> AllNoteOnEvent::mpcEventToBytes(std::shared_ptr<NoteOnEvent> event)
{
    std::vector<char> bytes(AllSequence::EVENT_SEG_LENGTH);
	
	bytes[NOTE_NUMBER_OFFSET] = static_cast<int8_t>(event->getNote());
	
    try {
		writeTrackNumber(bytes, event->getTrack());
		writeVariationType(bytes, event->getVariationType());
		writeVariationValue(bytes, event->getVariationValue());
		AllEvent::writeTick(bytes, static_cast<int>(event->getTick()));
		writeDuration(bytes, static_cast<int>(event->getDuration()));
        writeVelocity(bytes, event->getVelocity());
	}
	catch (const std::exception& e) {
		throw e;
	}
    
    return bytes;
}

std::vector<int> AllNoteOnEvent::DURATION_BYTE1_BIT_RANGE = std::vector<int>{ 4, 7 };
std::vector<int> AllNoteOnEvent::DURATION_BYTE2_BIT_RANGE = std::vector<int>{ 6, 7 };
std::vector<int> AllNoteOnEvent::TRACK_NUMBER_BIT_RANGE = std::vector<int>{ 0, 5 };
std::vector<int> AllNoteOnEvent::VELOCITY_BIT_RANGE = std::vector<int>{ 0, 6 };
std::vector<int> AllNoteOnEvent::VAR_VALUE_BIT_RANGE = std::vector<int>{ 0, 6 };

int AllNoteOnEvent::readDuration(const std::vector<char>& bytes)
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

int AllNoteOnEvent::readTrackNumber(const std::vector<char>& bytes)
{
    auto b = static_cast<unsigned char>(bytes[TRACK_NUMBER_OFFSET]);
    for (int i = 0; i < 8; i++)
    {
        if (i < TRACK_NUMBER_BIT_RANGE[0] || i > TRACK_NUMBER_BIT_RANGE[1])
        {
            b = BitUtil::setBit(static_cast<unsigned char>(b), i, false);
        }
    }
    return b;
}

int AllNoteOnEvent::readVelocity(const std::vector<char>& bytes)
{
    auto b = static_cast<unsigned char>(bytes[VELOCITY_OFFSET]);
    for (int i = 0; i < 8; i++)
    {
        if (i < VELOCITY_BIT_RANGE[0] || i > VELOCITY_BIT_RANGE[1])
        {
            b = BitUtil::setBit(static_cast<unsigned char>(b), i, false);
        }
    }
    return b;
}

int AllNoteOnEvent::readVariationValue(const std::vector<char>& bytes)
{
    auto b = static_cast<unsigned char>(bytes[VAR_VALUE_OFFSET]);
    for (int i = 0; i < 8; i++)
    {
        if (i < VAR_VALUE_BIT_RANGE[0] || i > VAR_VALUE_BIT_RANGE[1])
        {
            b = BitUtil::setBit(static_cast<unsigned char>(b), i, false);
        }
    }
    return b;
}

NoteOnEvent::VARIATION_TYPE AllNoteOnEvent::readVariationType(const std::vector<char>& bytes)
{
    auto byte1 = bytes[VAR_TYPE_BYTE1_OFFSET];
    auto byte2 = bytes[VAR_TYPE_BYTE2_OFFSET];
    
    auto b1 = BitUtil::isBitOn(byte1, VAR_TYPE_BYTE1_BIT);
    auto b2 = BitUtil::isBitOn(byte2, VAR_TYPE_BYTE2_BIT);
 
    if (b1 && b2)
        return NoteOnEvent::VARIATION_TYPE::FILTER_3;

    else if (b1 && !b2)
        return NoteOnEvent::VARIATION_TYPE::ATTACK_2;

    else if (!b1 && b2)
        return NoteOnEvent::VARIATION_TYPE::DECAY_1;

    else
        return NoteOnEvent::VARIATION_TYPE::TUNE_0;
}

void AllNoteOnEvent::writeVelocity(std::vector<char>& event, int v)
{
    auto value = static_cast< int8_t >(v);
    event[VELOCITY_OFFSET] = BitUtil::stitchBytes(event[VELOCITY_OFFSET], std::vector<int>{ VAR_TYPE_BYTE1_BIT, VAR_TYPE_BYTE1_BIT }, value, VELOCITY_BIT_RANGE);
}

void AllNoteOnEvent::writeTrackNumber(std::vector<char>& event, int t)
{
    auto value = static_cast< int8_t >(t);
    event[TRACK_NUMBER_OFFSET] = BitUtil::stitchBytes(event[TRACK_NUMBER_OFFSET], DURATION_BYTE2_BIT_RANGE, value, TRACK_NUMBER_BIT_RANGE);
}

void AllNoteOnEvent::writeVariationValue(std::vector<char>& event, int v)
{
    auto value = static_cast< int8_t >(v);
    event[VAR_VALUE_OFFSET] = BitUtil::stitchBytes(event[VAR_VALUE_OFFSET], std::vector<int>{ VAR_TYPE_BYTE2_BIT, VAR_TYPE_BYTE2_BIT }, value, VAR_VALUE_BIT_RANGE);
}

void AllNoteOnEvent::writeDuration(std::vector<char>& event, int duration)
{
    auto s1 = static_cast< short >(duration >> 6);
    auto s2 = static_cast< short >(duration >> 2);
    auto s3 = static_cast< short >(duration & 255);
    
    event[DURATION_BYTE1_OFFSET] = BitUtil::stitchBytes(event[DURATION_BYTE1_OFFSET], AllEvent::TICK_BYTE3_BIT_RANGE, static_cast< int8_t >(s1), DURATION_BYTE1_BIT_RANGE);
    
    event[DURATION_BYTE2_OFFSET] = BitUtil::stitchBytes(event[DURATION_BYTE2_OFFSET], TRACK_NUMBER_BIT_RANGE, static_cast< int8_t >(s2), DURATION_BYTE2_BIT_RANGE);

    event[DURATION_BYTE3_OFFSET] = static_cast< int8_t >(s3);
}

void AllNoteOnEvent::writeVariationType(std::vector<char>& event, int type)
{
    auto byte1 = event[VAR_TYPE_BYTE1_OFFSET];
    auto byte2 = event[VAR_TYPE_BYTE2_OFFSET];
    byte1 = static_cast<char>(BitUtil::setBit(byte1, VAR_TYPE_BYTE1_BIT, type == 2 || type == 3));
    byte2 = static_cast<char>(BitUtil::setBit(byte2, VAR_TYPE_BYTE2_BIT, type == 1 || type == 3));
    event[VAR_TYPE_BYTE1_OFFSET] = byte1;
    event[VAR_TYPE_BYTE2_OFFSET] = byte2;
}
