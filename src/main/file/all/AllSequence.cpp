#include "AllSequence.hpp"

#include "Bar.hpp"
#include "AllEvent.hpp"
#include "AllParser.hpp"
#include "BarList.hpp"
#include "SequenceNames.hpp"
#include "Tracks.hpp"

#include "sequencer/EventRef.hpp"
#include "sequencer/MixerEvent.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/SystemExclusiveEvent.hpp"

#include "file/ByteUtil.hpp"
#include "StrUtil.hpp"
#include "Util.hpp"
#include "sequencer/SequencerStateManager.hpp"

#ifdef __linux__
#include <climits>
#endif

using namespace mpc::file::all;
using namespace mpc::sequencer;

AllSequence::AllSequence(const std::vector<char> &bytes)
{
    barList = new BarList(Util::vecCopyOfRange(
        bytes, BAR_LIST_OFFSET, BAR_LIST_OFFSET + BAR_LIST_LENGTH));
    const auto nameBytes = Util::vecCopyOfRange(
        bytes, NAME_OFFSET, NAME_OFFSET + AllParser::NAME_LENGTH);
    name = "";

    for (const char c : nameBytes)
    {
        if (c == 0x00)
        {
            break;
        }

        name.push_back(c);
    }

    name = StrUtil::trim(name);

    tempo = getTempoDouble(
        std::vector{bytes[TEMPO_BYTE1_OFFSET], bytes[TEMPO_BYTE2_OFFSET]});
    const auto barCountBytes = std::vector{bytes[BAR_COUNT_BYTE1_OFFSET],
                                           bytes[BAR_COUNT_BYTE2_OFFSET]};
    barCount = ByteUtil::bytes2ushort(barCountBytes);
    loopFirst = ByteUtil::bytes2ushort(
        std::vector{bytes[LOOP_FIRST_OFFSET], bytes[LOOP_FIRST_OFFSET + 1]});
    loopLast = ByteUtil::bytes2ushort(
        std::vector{bytes[LOOP_LAST_OFFSET], bytes[LOOP_LAST_OFFSET + 1]});

    if (loopLast > 998)
    {
        loopLast = barCount;
        loopLastEnd = true;
    }

    loop = bytes[LOOP_ENABLED_OFFSET] > 0;

    startTime.hours = bytes[START_TIME_OFFSET];
    startTime.minutes = bytes[START_TIME_OFFSET + 1];
    startTime.seconds = bytes[START_TIME_OFFSET + 2];
    startTime.frames = bytes[START_TIME_OFFSET + 3];
    startTime.frameDecimals = bytes[START_TIME_OFFSET + 4];

    for (int i = 0; i < 33; i++)
    {
        const auto offset =
            DEVICE_NAMES_OFFSET + i * AllParser::DEV_NAME_LENGTH;
        std::string stringBuffer;
        auto stringBytes = Util::vecCopyOfRange(
            bytes, offset, offset + AllParser::DEV_NAME_LENGTH);

        for (const char c : stringBytes)
        {
            if (c == 0x00)
            {
                break;
            }

            stringBuffer.push_back(c);
        }

        devNames[i] = stringBuffer;
    }

    tracks =
        new Tracks(Util::vecCopyOfRange(bytes, TRACKS_OFFSET, TRACKS_LENGTH));
    allEvents = readEvents(bytes);
}

AllSequence::~AllSequence()
{
    if (tracks != nullptr)
    {
        delete tracks;
    }

    if (barList != nullptr)
    {
        delete barList;
    }
}

void AllSequence::applyToMpcSeq(const std::shared_ptr<Sequence> &mpcSeq,
                                const SequencerStateManager *manager) const
{
    mpcSeq->init(barCount - 1);

    for (int i = 0; i < barCount; i++)
    {
        const auto num = barList->getBars()[i]->getNumerator();
        const auto den = barList->getBars()[i]->getDenominator();
        mpcSeq->setTimeSignature(i, num, den);
    }

    mpcSeq->setName(name);
    mpcSeq->setInitialTempo(tempo);
    const auto at = tracks;

    for (int i = 0; i < Mpc2000XlSpecs::TRACK_COUNT; ++i)
    {
        const auto t = mpcSeq->getTrack(i);

        constexpr bool updateUsedness = false;

        t->setName(at->getName(i));

        t->setDeviceIndex(at->getDevice(i), updateUsedness);
        t->setBusType(busIndexToBusType(at->getBus(i)), updateUsedness);
        t->setProgramChange(at->getPgm(i), updateUsedness);
        t->setOn(at->isOn(i), updateUsedness);
        t->setVelocityRatio(at->getVelo(i), updateUsedness);
        t->setTransmitProgramChangesEnabled(at->isTransmitProgramChangesEnabled(i));

        manager->enqueue(SetTrackUsed{mpcSeq->getSequenceIndex(), TrackIndex(i),
                                      at->isUsed(i)});
    }

    for (int j = 0; j < getEventAmount(); j++)
    {
        auto e = allEvents[j];

        if (e.type == EventType::Unknown)
        {
            continue;
        }

        const int track = e.trackIndex;

        mpcSeq->getTrack(track)->acquireAndInsertEvent(e);
    }

    for (int i = 0; i < 32; i++)
    {
        mpcSeq->setDeviceName(i, devNames[i]);
    }

    mpcSeq->setFirstLoopBarIndex(BarIndex(loopFirst));
    mpcSeq->setLastLoopBarIndex(BarIndex(loopLast));
    mpcSeq->setLastLoopBarIndex(BarIndex(loopLast));

    if (loopLastEnd)
    {
        mpcSeq->setLastLoopBarIndex(EndOfSequence);
    }

    mpcSeq->setLoopEnabled(loop);
    mpcSeq->getStartTime().hours = startTime.hours;
    mpcSeq->getStartTime().minutes = startTime.minutes;
    mpcSeq->getStartTime().seconds = startTime.seconds;
    mpcSeq->getStartTime().frames = startTime.frames;
    mpcSeq->getStartTime().frameDecimals = startTime.frameDecimals;
}

AllSequence::AllSequence(Sequence *seq, int number)
{
    auto segmentCountLastEventIndex = SequenceNames::getSegmentCount(seq);
    auto segmentCount = getSegmentCount(seq);
    auto terminatorCount = (segmentCount & 1) == 0 ? 2 : 1;
    saveBytes = std::vector<char>(10240 + segmentCount * EVENT_SEG_LENGTH +
                                  terminatorCount * EVENT_SEG_LENGTH);

    for (int i = 0; i < AllParser::NAME_LENGTH; i++)
    {
        saveBytes[i] =
            StrUtil::padRight(seq->getName(), " ", AllParser::NAME_LENGTH)[i];
    }

    if ((segmentCountLastEventIndex & 1) != 0)
    {
        segmentCountLastEventIndex--;
    }

    segmentCountLastEventIndex /= 2;
    auto lastEventIndexBytes = ByteUtil::ushort2bytes(
        1 + (segmentCountLastEventIndex < 0 ? 0 : segmentCountLastEventIndex));

    saveBytes[LAST_EVENT_INDEX_OFFSET] = lastEventIndexBytes[0];
    saveBytes[LAST_EVENT_INDEX_OFFSET + 1] = lastEventIndexBytes[1];

    for (int i = PADDING1_OFFSET; i < PADDING1_OFFSET + PADDING1.size(); i++)
    {
        saveBytes[i] = PADDING1[i - PADDING1_OFFSET];
    }

    setTempoDouble(seq->getInitialTempo());

    for (int i = PADDING2_OFFSET; i < PADDING2_OFFSET + PADDING2.size(); i++)
    {
        saveBytes[i] = PADDING2[i - PADDING2_OFFSET];
    }

    setBarCount(seq->getLastBarIndex() + 1);
    setLastTick(seq);
    saveBytes[SEQUENCE_INDEX_OFFSET] = number;
    setUnknown32BitInt(seq);
    auto loopStartBytes = ByteUtil::ushort2bytes(seq->getFirstLoopBarIndex());
    auto loopEndBytes = ByteUtil::ushort2bytes(seq->getLastLoopBarIndex());

    if (seq->getLastLoopBarIndex() == EndOfSequence)
    {
        loopEndBytes =
            std::vector{static_cast<char>(255), static_cast<char>(255)};
    }

    saveBytes[LOOP_FIRST_OFFSET] = loopStartBytes[0];
    saveBytes[LOOP_FIRST_OFFSET + 1] = loopStartBytes[1];
    saveBytes[LOOP_LAST_OFFSET] = loopEndBytes[0];
    saveBytes[LOOP_LAST_OFFSET + 1] = loopEndBytes[1];
    saveBytes[LOOP_ENABLED_OFFSET] = seq->isLoopEnabled() ? 1 : 0;

    saveBytes[START_TIME_OFFSET] = seq->getStartTime().hours;
    saveBytes[START_TIME_OFFSET + 1] = seq->getStartTime().minutes;
    saveBytes[START_TIME_OFFSET + 2] = seq->getStartTime().seconds;
    saveBytes[START_TIME_OFFSET + 3] = seq->getStartTime().frames;
    saveBytes[START_TIME_OFFSET + 4] = seq->getStartTime().frameDecimals;

    for (int i = 0; i < PADDING4.size(); i++)
    {
        saveBytes[PADDING4_OFFSET + i] = PADDING4[i];
    }

    for (int i = 0; i < 33; i++)
    {
        auto offset = DEVICE_NAMES_OFFSET + i * AllParser::DEV_NAME_LENGTH;

        for (int j = 0; j < AllParser::DEV_NAME_LENGTH; j++)
        {
            saveBytes[offset + j] = StrUtil::padRight(
                seq->getDeviceName(i), " ", AllParser::DEV_NAME_LENGTH)[j];
        }
    }
    Tracks allFileSeqTracks(seq);

    for (int i = 0; i < TRACKS_LENGTH; i++)
    {
        saveBytes[i + TRACKS_OFFSET] = allFileSeqTracks.getBytes()[i];
    }

    BarList allFileBarList(seq);

    for (int i = BAR_LIST_OFFSET; i < BAR_LIST_OFFSET + BAR_LIST_LENGTH; i++)
    {
        saveBytes[i] = allFileBarList.getBytes()[i - BAR_LIST_OFFSET];
    }

    auto eventArraysChunk = createEventSegmentsChunk(seq);

    for (int i = EVENTS_OFFSET; i < EVENTS_OFFSET + eventArraysChunk.size();
         i++)
    {
        saveBytes[i] = eventArraysChunk[i - EVENTS_OFFSET];
    }

    for (int i = static_cast<int>(saveBytes.size()) - 8; i < saveBytes.size();
         i++)
    {
        saveBytes[i] = static_cast<char>(255);
    }
}

const int AllSequence::MAX_SYSEX_SIZE;
const int AllSequence::EVENT_ID_OFFSET;
const char AllSequence::POLY_PRESSURE_ID;
const char AllSequence::CONTROL_CHANGE_ID;
const char AllSequence::PGM_CHANGE_ID;
const char AllSequence::CH_PRESSURE_ID;
const char AllSequence::PITCH_BEND_ID;
const char AllSequence::SYS_EX_ID;
const char AllSequence::SYS_EX_TERMINATOR_ID;
std::vector<char> AllSequence::TERMINATOR = std::vector{
    static_cast<char>(0xFF), static_cast<char>(0xFF), static_cast<char>(0xFF),
    static_cast<char>(0xFF), static_cast<char>(0xFF), static_cast<char>(0xFF),
    static_cast<char>(0xFF), static_cast<char>(0xFF)};
const int AllSequence::MAX_EVENT_SEG_COUNT;
const int AllSequence::EVENT_SEG_LENGTH;
const int AllSequence::NAME_OFFSET;
const int AllSequence::LAST_EVENT_INDEX_OFFSET;
const int AllSequence::SEQUENCE_INDEX_OFFSET;
const int AllSequence::PADDING1_OFFSET;
std::vector<char> AllSequence::PADDING1 = std::vector<char>{1, 1, 0};
const int AllSequence::TEMPO_BYTE1_OFFSET;
const int AllSequence::TEMPO_BYTE2_OFFSET;
const int AllSequence::PADDING2_OFFSET;
std::vector<char> AllSequence::PADDING2 = std::vector<char>{4, 4};
const int AllSequence::BAR_COUNT_BYTE1_OFFSET;
const int AllSequence::BAR_COUNT_BYTE2_OFFSET;
const int AllSequence::LAST_TICK_BYTE1_OFFSET;
const int AllSequence::LAST_TICK_BYTE2_OFFSET;
const int AllSequence::UNKNOWN32_BIT_INT_OFFSET;
const int AllSequence::LOOP_FIRST_OFFSET;
const int AllSequence::LOOP_LAST_OFFSET;
const int AllSequence::LOOP_ENABLED_OFFSET;
const int AllSequence::PADDING4_OFFSET;
std::vector<char> AllSequence::PADDING4 =
    std::vector<char>{40, 0, static_cast<char>(128), 0, 0};
const int AllSequence::LAST_TICK_BYTE3_OFFSET;
const int AllSequence::LAST_TICK_BYTE4_OFFSET;
const int AllSequence::DEVICE_NAMES_OFFSET;
const int AllSequence::TRACKS_OFFSET;
const int AllSequence::TRACKS_LENGTH;
const int AllSequence::BAR_LIST_OFFSET;
const int AllSequence::BAR_LIST_LENGTH;
const int AllSequence::EVENTS_OFFSET;

std::vector<EventData>
AllSequence::readEvents(const std::vector<char> &seqBytes)
{
    std::vector<EventData> events;

    for (auto &eventBytes : readEventSegments(seqBytes))
    {
        events.emplace_back(AllEvent::bytesToMpcEvent(eventBytes));
    }

    return events;
}

std::vector<std::vector<char>>
AllSequence::readEventSegments(const std::vector<char> &seqBytes)
{
    std::vector<std::vector<char>> eventArrays;
    int candidateOffset = EVENTS_OFFSET;

    for (int i = 0; i < MAX_EVENT_SEG_COUNT; i++)
    {
        auto ea = Util::vecCopyOfRange(seqBytes, candidateOffset,
                                       candidateOffset + EVENT_SEG_LENGTH);

        if (Util::vecEquals(ea, TERMINATOR))
        {
            break;
        }

        if (ea[EVENT_ID_OFFSET] == SYS_EX_ID)
        {
            int sysexSegs;
            for (sysexSegs = 0; sysexSegs < MAX_SYSEX_SIZE; sysexSegs++)
            {
                auto potentialTerminator = Util::vecCopyOfRange(
                    seqBytes, candidateOffset + sysexSegs * EVENT_SEG_LENGTH,
                    candidateOffset + sysexSegs * EVENT_SEG_LENGTH +
                        EVENT_SEG_LENGTH);

                if (potentialTerminator[EVENT_ID_OFFSET] ==
                    SYS_EX_TERMINATOR_ID)
                {
                    break;
                }
            }

            sysexSegs++;
            ea = Util::vecCopyOfRange(seqBytes, candidateOffset,
                                      candidateOffset +
                                          sysexSegs * EVENT_SEG_LENGTH);
        }
        eventArrays.push_back(ea);
        candidateOffset += static_cast<int>(ea.size());
    }
    return eventArrays;
}

double AllSequence::getTempoDouble(const std::vector<char> &bytePair)
{
    const auto s = ByteUtil::bytes2ushort(bytePair);
    return static_cast<double>(s) / 10.0;
}

int AllSequence::getNumberOfEventSegmentsForThisSeq(
    const std::vector<char> &seqBytes)
{
    auto accum = 0;

    for (auto &ba : readEventSegments(seqBytes))
    {
        accum += static_cast<int>(ba.size()) / 8;
    }

    return accum;
}

int AllSequence::getEventAmount() const
{
    return static_cast<int>(allEvents.size());
}

int AllSequence::getSegmentCount(Sequence *seq)
{
    int segmentCount = 0;

    for (const auto &track : seq->getTracks())
    {
        if (track->getIndex() > 63)
        {
            break;
        }

        for (auto &e : track->getEvents())
        {
            if (const auto sysExEvent =
                    std::dynamic_pointer_cast<SystemExclusiveEvent>(e);
                sysExEvent)
            {
                const int dataSegments = static_cast<int>(ceil(
                    static_cast<int>(sysExEvent->getBytes().size()) / 8.0));
                segmentCount += dataSegments + 2;
            }
            else if (const auto mixerEvent =
                         std::dynamic_pointer_cast<MixerEvent>(e);
                     mixerEvent)
            {
                segmentCount += 4;
            }
            else
            {
                segmentCount++;
            }
        }
    }
    return segmentCount;
}

void AllSequence::setUnknown32BitInt(const Sequence *seq)
{
    const auto unknownNumberBytes1 = ByteUtil::uint2bytes(10000000);
    const auto unknownNumberBytes2 =
        ByteUtil::uint2bytes(seq->getLastTick() * 5208.333333333333);

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            saveBytes[UNKNOWN32_BIT_INT_OFFSET + j + i * 4] =
                unknownNumberBytes1[j];
        }
    }

    for (int i = 2; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            saveBytes[UNKNOWN32_BIT_INT_OFFSET + j + i * 4] =
                unknownNumberBytes2[j];
        }
    }
}

void AllSequence::setBarCount(const int i)
{
    const auto ba = ByteUtil::ushort2bytes(i);
    saveBytes[BAR_COUNT_BYTE1_OFFSET] = ba[0];
    saveBytes[BAR_COUNT_BYTE2_OFFSET] = ba[1];
}

std::vector<char> AllSequence::createEventSegmentsChunk(Sequence *seq)
{
    std::vector<std::vector<char>> ea;

    for (int i = 0; i < seq->getLastTick(); i++)
    {
        for (const auto &track : seq->getTracks())
        {
            if (track->getIndex() > 63)
            {
                break;
            }

            for (const auto &event : track->getEvents())
            {
                if (event->getTick() == i)
                {
                    event->setTrack(track->getIndex());
                    ea.push_back(AllEvent::mpcEventToBytes(*event->handle));
                }
            }
        }
    }

    ea.push_back(TERMINATOR);
    return ByteUtil::stitchByteArrays(ea);
}

void AllSequence::setTempoDouble(const double tempoForSaveBytes)
{
    const auto ba = ByteUtil::ushort2bytes(
        static_cast<unsigned short>(tempoForSaveBytes * 10.0));
    saveBytes[TEMPO_BYTE1_OFFSET] = ba[0];
    saveBytes[TEMPO_BYTE2_OFFSET] = ba[1];
}

void AllSequence::setLastTick(const Sequence *seq)
{
    const auto lastTick = seq->getLastTick();
    const auto remainder = lastTick % 65536;
    const auto b = ByteUtil::ushort2bytes(remainder);
    const auto large = static_cast<int>(floor(lastTick / 65536.0));
    saveBytes[LAST_TICK_BYTE1_OFFSET] = b[0];
    saveBytes[LAST_TICK_BYTE2_OFFSET] = b[1];
    saveBytes[LAST_TICK_BYTE2_OFFSET + 1] = large;
    saveBytes[LAST_TICK_BYTE3_OFFSET] = b[0];
    saveBytes[LAST_TICK_BYTE4_OFFSET] = b[1];
    saveBytes[LAST_TICK_BYTE4_OFFSET + 1] = large;
}

std::vector<char> &AllSequence::getBytes()
{
    return saveBytes;
}
