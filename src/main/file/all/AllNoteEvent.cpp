#include "AllNoteEvent.hpp"

#include "AllEvent.hpp"
#include "AllSequence.hpp"

#include "sequencer/NoteEvent.hpp"

#include "file/BitUtil.hpp"

using namespace mpc::file::all;
using namespace mpc::sequencer;

std::shared_ptr<NoteOnEvent>
AllNoteOnEvent::bytesToMpcEvent(const std::vector<char> &bytes)
{
    auto event = std::make_shared<NoteOnEvent>();

    event->setNote(NoteNumber(bytes[NOTE_NUMBER_OFFSET]));
    event->setTick(AllEvent::readTick(bytes));
    const auto track = readTrackNumber(bytes);
    event->setTrack(TrackIndex(track));
    event->setDuration(readDuration(bytes) - track * 4);
    event->setVelocity(Velocity(readVelocity(bytes)));
    event->setVariationType(readVariationType(bytes));
    event->setVariationValue(readVariationValue(bytes));

    return event;
}

std::vector<char>
AllNoteOnEvent::mpcEventToBytes(const std::shared_ptr<NoteOnEvent> &event)
{
    std::vector<char> bytes(AllSequence::EVENT_SEG_LENGTH);

    bytes[NOTE_NUMBER_OFFSET] = static_cast<int8_t>(event->getNote());

    try
    {
        writeTrackNumber(bytes, event->getTrack());
        writeVariationType(bytes, event->getVariationType());
        writeVariationValue(bytes, event->getVariationValue());
        AllEvent::writeTick(bytes, event->getTick());
        writeDuration(bytes, event->getDuration().value_or(-1));
        writeVelocity(bytes, event->getVelocity());
    }
    catch (const std::exception &e)
    {
        throw e;
    }

    return bytes;
}

std::vector<int> AllNoteOnEvent::DURATION_BYTE1_BIT_RANGE = std::vector{4, 7};
std::vector<int> AllNoteOnEvent::DURATION_BYTE2_BIT_RANGE = std::vector{6, 7};
std::vector<int> AllNoteOnEvent::TRACK_NUMBER_BIT_RANGE = std::vector{0, 5};
std::vector<int> AllNoteOnEvent::VELOCITY_BIT_RANGE = std::vector{0, 6};
std::vector<int> AllNoteOnEvent::VAR_VALUE_BIT_RANGE = std::vector{0, 6};

int AllNoteOnEvent::readDuration(const std::vector<char> &bytes)
{
    auto b1 = bytes[DURATION_BYTE1_OFFSET];
    auto b2 = bytes[DURATION_BYTE2_OFFSET];
    const auto b3 = bytes[DURATION_BYTE3_OFFSET];

    if (static_cast<unsigned char>(b1) == 255 &&
        static_cast<unsigned char>(b2) == 255 &&
        static_cast<unsigned char>(b3) == 255)
    {
        return -1;
    }

    b1 = BitUtil::removeUnusedBits(b1, DURATION_BYTE1_BIT_RANGE);
    b2 = BitUtil::removeUnusedBits(b2, DURATION_BYTE2_BIT_RANGE);

    const auto i1 = b1 & 255;
    const auto i2 = b2 & 255;
    const auto i3 = b3 & 255;

    return (i1 << 6) + (i2 << 2) + i3;
}

int AllNoteOnEvent::readTrackNumber(const std::vector<char> &bytes)
{
    auto b = static_cast<unsigned char>(bytes[TRACK_NUMBER_OFFSET]);
    for (int i = 0; i < 8; i++)
    {
        if (i < TRACK_NUMBER_BIT_RANGE[0] || i > TRACK_NUMBER_BIT_RANGE[1])
        {
            b = BitUtil::setBit(b, i, false);
        }
    }
    return b;
}

int AllNoteOnEvent::readVelocity(const std::vector<char> &bytes)
{
    auto b = static_cast<unsigned char>(bytes[VELOCITY_OFFSET]);
    for (int i = 0; i < 8; i++)
    {
        if (i < VELOCITY_BIT_RANGE[0] || i > VELOCITY_BIT_RANGE[1])
        {
            b = BitUtil::setBit(b, i, false);
        }
    }
    return b;
}

int AllNoteOnEvent::readVariationValue(const std::vector<char> &bytes)
{
    auto b = static_cast<unsigned char>(bytes[VAR_VALUE_OFFSET]);
    for (int i = 0; i < 8; i++)
    {
        if (i < VAR_VALUE_BIT_RANGE[0] || i > VAR_VALUE_BIT_RANGE[1])
        {
            b = BitUtil::setBit(b, i, false);
        }
    }
    return b;
}

NoteOnEvent::VARIATION_TYPE
AllNoteOnEvent::readVariationType(const std::vector<char> &bytes)
{
    const auto byte1 = bytes[VAR_TYPE_BYTE1_OFFSET];
    const auto byte2 = bytes[VAR_TYPE_BYTE2_OFFSET];

    const auto b1 = BitUtil::isBitOn(byte1, VAR_TYPE_BYTE1_BIT);
    const auto b2 = BitUtil::isBitOn(byte2, VAR_TYPE_BYTE2_BIT);

    if (b1 && b2)
    {
        return NoteOnEvent::VARIATION_TYPE::FILTER_3;
    }
    if (b1 && !b2)
    {
        return NoteOnEvent::VARIATION_TYPE::ATTACK_2;
    }
    if (!b1 && b2)
    {
        return NoteOnEvent::VARIATION_TYPE::DECAY_1;
    }
    return NoteOnEvent::VARIATION_TYPE::TUNE_0;
}

void AllNoteOnEvent::writeVelocity(std::vector<char> &event, int v)
{
    const auto value = static_cast<int8_t>(v);
    event[VELOCITY_OFFSET] = BitUtil::stitchBytes(
        event[VELOCITY_OFFSET],
        std::vector{VAR_TYPE_BYTE1_BIT, VAR_TYPE_BYTE1_BIT}, value,
        VELOCITY_BIT_RANGE);
}

void AllNoteOnEvent::writeTrackNumber(std::vector<char> &event, int t)
{
    const auto value = static_cast<int8_t>(t);
    event[TRACK_NUMBER_OFFSET] = BitUtil::stitchBytes(
        event[TRACK_NUMBER_OFFSET], DURATION_BYTE2_BIT_RANGE, value,
        TRACK_NUMBER_BIT_RANGE);
}

void AllNoteOnEvent::writeVariationValue(std::vector<char> &event, int v)
{
    const auto value = static_cast<int8_t>(v);
    event[VAR_VALUE_OFFSET] = BitUtil::stitchBytes(
        event[VAR_VALUE_OFFSET],
        std::vector{VAR_TYPE_BYTE2_BIT, VAR_TYPE_BYTE2_BIT}, value,
        VAR_VALUE_BIT_RANGE);
}

void AllNoteOnEvent::writeDuration(std::vector<char> &event, int duration)
{
    const auto s1 = static_cast<short>(duration >> 6);
    const auto s2 = static_cast<short>(duration >> 2);
    const auto s3 = static_cast<short>(duration & 255);

    event[DURATION_BYTE1_OFFSET] = BitUtil::stitchBytes(
        event[DURATION_BYTE1_OFFSET], AllEvent::TICK_BYTE3_BIT_RANGE,
        static_cast<int8_t>(s1), DURATION_BYTE1_BIT_RANGE);

    event[DURATION_BYTE2_OFFSET] = BitUtil::stitchBytes(
        event[DURATION_BYTE2_OFFSET], TRACK_NUMBER_BIT_RANGE,
        static_cast<int8_t>(s2), DURATION_BYTE2_BIT_RANGE);

    event[DURATION_BYTE3_OFFSET] = static_cast<int8_t>(s3);
}

void AllNoteOnEvent::writeVariationType(std::vector<char> &event, int type)
{
    auto byte1 = event[VAR_TYPE_BYTE1_OFFSET];
    auto byte2 = event[VAR_TYPE_BYTE2_OFFSET];
    byte1 = static_cast<char>(
        BitUtil::setBit(byte1, VAR_TYPE_BYTE1_BIT, type == 2 || type == 3));
    byte2 = static_cast<char>(
        BitUtil::setBit(byte2, VAR_TYPE_BYTE2_BIT, type == 1 || type == 3));
    event[VAR_TYPE_BYTE1_OFFSET] = byte1;
    event[VAR_TYPE_BYTE2_OFFSET] = byte2;
}
