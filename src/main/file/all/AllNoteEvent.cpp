#include <file/all/AllNoteEvent.hpp>

#include <file/BitUtil.hpp>
#include <file/all/AllEvent.hpp>
#include <file/all/AllSequence.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/NoteEvent.hpp>

using namespace std;
using namespace mpc::file::all;

AllNoteEvent::AllNoteEvent(vector<char> b) 
{
	note = b[NOTE_NUMBER_OFFSET];
	tick = AllEvent::readTick(b);
	track = readTrackNumber(b);
	duration = readDuration(b) - (track * 4);
	velocity = readVelocity(b);
	variationValue = readVariationValue(b);
	variationType = readVariationType(b);
}

AllNoteEvent::AllNoteEvent(mpc::sequencer::Event* event)
{
	saveBytes = vector<char>(Sequence::EVENT_SEG_LENGTH);
	auto ne = dynamic_cast<sequencer::NoteEvent*>(event);
	saveBytes[NOTE_NUMBER_OFFSET] = static_cast<int8_t>(ne->getNote());
	try {
		saveBytes = setVelocity(saveBytes, ne->getVelocity());
		saveBytes = setTrackNumber(saveBytes, ne->getTrack());
		saveBytes = setVariationType(saveBytes, ne->getVariationType());
		saveBytes = setVariationValue(saveBytes, ne->getVariationValue());
		saveBytes = AllEvent::writeTick(saveBytes, static_cast<int>(ne->getTick()));
		saveBytes = setDuration(saveBytes, static_cast<int>(ne->getDuration()));
	}
	catch (exception e) {
		throw e;
	}
}

const int AllNoteEvent::DURATION_BYTE1_OFFSET;
vector<int> AllNoteEvent::DURATION_BYTE1_BIT_RANGE = vector<int>{ 4, 7 };
const int AllNoteEvent::DURATION_BYTE2_OFFSET;
vector<int> AllNoteEvent::DURATION_BYTE2_BIT_RANGE = vector<int>{ 6, 7 };
const int AllNoteEvent::TRACK_NUMBER_OFFSET;
vector<int> AllNoteEvent::TRACK_NUMBER_BIT_RANGE = vector<int>{ 0, 5 };
const int AllNoteEvent::NOTE_NUMBER_OFFSET;
const int AllNoteEvent::DURATION_BYTE3_OFFSET;
const int AllNoteEvent::VELOCITY_OFFSET;
vector<int> AllNoteEvent::VELOCITY_BIT_RANGE = vector<int>{ 0, 6 };
const int AllNoteEvent::VAR_TYPE_BYTE1_OFFSET;
const int AllNoteEvent::VAR_TYPE_BYTE1_BIT;
const int AllNoteEvent::VAR_TYPE_BYTE2_OFFSET;
const int AllNoteEvent::VAR_TYPE_BYTE2_BIT;
const int AllNoteEvent::VAR_VALUE_OFFSET;
vector<int> AllNoteEvent::VAR_VALUE_BIT_RANGE = vector<int>{ 0, 6 };

int AllNoteEvent::getNote()
{
    return note;
}

int AllNoteEvent::getTick()
{
    return tick;
}

int AllNoteEvent::getDuration()
{
    return duration;
}

int AllNoteEvent::getTrack()
{
    return track;
}

int AllNoteEvent::getVelocity()
{
    return velocity;
}

int AllNoteEvent::getVariationType()
{
    return variationType;
}

int AllNoteEvent::getVariationValue()
{
    return variationValue;
}

int AllNoteEvent::readDuration(vector<char> b)
{
    auto b1 = b[DURATION_BYTE1_OFFSET];
    auto b2 = b[DURATION_BYTE2_OFFSET];
    auto b3 = b[DURATION_BYTE3_OFFSET];
    if (static_cast<unsigned char>(b1) == 255 && static_cast<unsigned char>(b2) == 255 && static_cast<unsigned char>(b3) == 255) {
        return -1;
    }

    b1 = moduru::file::BitUtil::removeUnusedBits(b1, DURATION_BYTE1_BIT_RANGE);
    b2 = moduru::file::BitUtil::removeUnusedBits(b2, DURATION_BYTE2_BIT_RANGE);
    auto i1 = static_cast<int>(b1 & 255);
    auto i2 = static_cast<int>(b2 & 255);
    auto i3 = static_cast<int>(b3 & 255);
    return (i1 << 6) + (i2 << 2) + i3;
}

int AllNoteEvent::readTrackNumber(vector<char> ba)
{
    auto b = moduru::file::BitUtil::removeUnusedBits(ba[TRACK_NUMBER_OFFSET], TRACK_NUMBER_BIT_RANGE);
    return b;
}

int AllNoteEvent::readVelocity(vector<char> ba)
{
    auto b = moduru::file::BitUtil::removeUnusedBits(ba[VELOCITY_OFFSET], VELOCITY_BIT_RANGE);
    return b;
}

int AllNoteEvent::readVariationValue(vector<char> ba)
{
    auto b = moduru::file::BitUtil::removeUnusedBits(ba[VAR_VALUE_OFFSET], VAR_VALUE_BIT_RANGE);
    return b;
}

int AllNoteEvent::readVariationType(vector<char> ba)
{
    auto byte1 = ba[VAR_TYPE_BYTE1_OFFSET];
    auto byte2 = ba[VAR_TYPE_BYTE2_OFFSET];
    auto b1 = moduru::file::BitUtil::isBitOn(byte1, VAR_TYPE_BYTE1_BIT);
    auto b2 = moduru::file::BitUtil::isBitOn(byte2, VAR_TYPE_BYTE2_BIT);
    if(b1 && b2)
        return 3;

    if(b1 && !b2)
        return 2;

    if(!b1 && b2)
        return 1;

    if(!b1 && !b2)
        return 0;

    return -1;
}

vector<char> AllNoteEvent::setVelocity(vector<char> event, int v)
{
    auto value = static_cast< int8_t >(v);
    event[VELOCITY_OFFSET] = moduru::file::BitUtil::stitchBytes(event[VELOCITY_OFFSET], vector<int>{ VAR_TYPE_BYTE1_BIT, VAR_TYPE_BYTE1_BIT }, value, VELOCITY_BIT_RANGE);
    return event;
}

vector<char> AllNoteEvent::setTrackNumber(vector<char> event, int t)
{
    auto value = static_cast< int8_t >(t);
    event[TRACK_NUMBER_OFFSET] = moduru::file::BitUtil::stitchBytes(event[TRACK_NUMBER_OFFSET], DURATION_BYTE2_BIT_RANGE, value, TRACK_NUMBER_BIT_RANGE);
    return event;
}

vector<char> AllNoteEvent::setVariationValue(vector<char> event, int v)
{
    auto value = static_cast< int8_t >(v);
    event[VAR_VALUE_OFFSET] = moduru::file::BitUtil::stitchBytes(event[VAR_VALUE_OFFSET], vector<int>{ VAR_TYPE_BYTE2_BIT, VAR_TYPE_BYTE2_BIT }, value, VAR_VALUE_BIT_RANGE);
    return event;
}

vector<char> AllNoteEvent::setDuration(vector<char> event, int duration)
{
    auto s1 = static_cast< short >(duration >> 6);
    auto s2 = static_cast< short >(duration >> 2);
    auto s3 = static_cast< short >(duration & 255);
    event[DURATION_BYTE1_OFFSET] = moduru::file::BitUtil::stitchBytes(event[DURATION_BYTE1_OFFSET], AllEvent::TICK_BYTE3_BIT_RANGE, static_cast< int8_t >(s1), DURATION_BYTE1_BIT_RANGE);
    event[DURATION_BYTE2_OFFSET] = moduru::file::BitUtil::stitchBytes(event[DURATION_BYTE2_OFFSET], TRACK_NUMBER_BIT_RANGE, static_cast< int8_t >(s2), DURATION_BYTE2_BIT_RANGE);
    event[DURATION_BYTE3_OFFSET] = static_cast< int8_t >(s3);
    return event;
}

vector<char> AllNoteEvent::setVariationType(vector<char> event, int type)
{
    auto byte1 = event[VAR_TYPE_BYTE1_OFFSET];
    auto byte2 = event[VAR_TYPE_BYTE2_OFFSET];
    switch (type) {
    case 0:
        moduru::file::BitUtil::setBit(byte1, VAR_TYPE_BYTE1_BIT, false);
        moduru::file::BitUtil::setBit(byte2, VAR_TYPE_BYTE2_BIT, false);
        break;
    case 1:
        moduru::file::BitUtil::setBit(byte1, VAR_TYPE_BYTE1_BIT, false);
        moduru::file::BitUtil::setBit(byte2, VAR_TYPE_BYTE2_BIT, true);
        break;
    case 2:
        moduru::file::BitUtil::setBit(byte1, VAR_TYPE_BYTE1_BIT, true);
        moduru::file::BitUtil::setBit(byte2, VAR_TYPE_BYTE2_BIT, false);
        break;
    case 3:
        moduru::file::BitUtil::setBit(byte1, VAR_TYPE_BYTE1_BIT, true);
        moduru::file::BitUtil::setBit(byte2, VAR_TYPE_BYTE2_BIT, true);
        break;
    }

    event[VAR_TYPE_BYTE1_OFFSET] = byte1;
    event[VAR_TYPE_BYTE2_OFFSET] = byte2;
    return event;
}

vector<char> AllNoteEvent::getBytes()
{
    return saveBytes;
}
