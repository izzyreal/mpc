#include "AllSequence.hpp"

#include "Bar.hpp"
#include "AllEvent.hpp"
#include "AllParser.hpp"
#include "BarList.hpp"
#include "SequenceNames.hpp"
#include "Tracks.hpp"

#include <sequencer/Event.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/SystemExclusiveEvent.hpp>
#include <sequencer/TempoChangeEvent.hpp>

#include <file/ByteUtil.hpp>
#include <StrUtil.hpp>
#include <Util.hpp>

#include <cmath>

#ifdef __linux__
#include <climits>
#endif

using namespace mpc::file::all;
using namespace mpc::sequencer;

AllSequence::AllSequence(const std::vector<char>& bytes)
{
    barList = new BarList(Util::vecCopyOfRange(bytes, BAR_LIST_OFFSET, BAR_LIST_OFFSET + BAR_LIST_LENGTH));
    auto nameBytes = Util::vecCopyOfRange(bytes, NAME_OFFSET, NAME_OFFSET + AllParser::NAME_LENGTH);
    name = "";
    
    for (char c : nameBytes)
    {
        if (c == 0x00)
            break;
        
        name.push_back(c);
    }
    
    tempo = getTempoDouble(std::vector<char>{ bytes[TEMPO_BYTE1_OFFSET] , bytes[TEMPO_BYTE2_OFFSET] });
    auto barCountBytes = std::vector<char>{ bytes[BAR_COUNT_BYTE1_OFFSET], bytes[BAR_COUNT_BYTE2_OFFSET] };
    barCount = ByteUtil::bytes2ushort(barCountBytes);
    loopFirst = ByteUtil::bytes2ushort(std::vector<char>{ bytes[LOOP_FIRST_OFFSET], bytes[LOOP_FIRST_OFFSET + 1] });
    loopLast = ByteUtil::bytes2ushort(std::vector<char>{ bytes[LOOP_LAST_OFFSET], bytes[LOOP_LAST_OFFSET + 1] });
    
    if (loopLast > 998)
    {
        loopLast = barCount;
        loopLastEnd = true;
    }
    
    loop = (bytes[LOOP_ENABLED_OFFSET] > 0);
    
    for (int i = 0; i < 33; i++)
    {
        auto offset = DEVICE_NAMES_OFFSET + (i * AllParser::DEV_NAME_LENGTH);
        std::string stringBuffer;
        auto stringBytes = Util::vecCopyOfRange(bytes, offset, offset + AllParser::DEV_NAME_LENGTH);
        
        for (char c : stringBytes)
        {
            if (c == 0x00)
                break;

            stringBuffer.push_back(c);
        }
        
        devNames[i] = stringBuffer;
    }
    
    tracks = new Tracks(Util::vecCopyOfRange(bytes, TRACKS_OFFSET, TRACKS_LENGTH));
    allEvents = readEvents(bytes);
}

AllSequence::~AllSequence()
{
    if (tracks != nullptr)
        delete tracks;

    if (barList != nullptr)
        delete barList;
}

void AllSequence::applyToMpcSeq(std::shared_ptr<mpc::sequencer::Sequence> mpcSeq)
{
    mpcSeq->init(barCount - 1);

    for (int i = 0; i < barCount; i++)
    {
        auto num = barList->getBars()[i]->getNumerator();
        auto den = barList->getBars()[i]->getDenominator();
        mpcSeq->setTimeSignature(i, num, den);
    }

    mpcSeq->setName(name);
    mpcSeq->setInitialTempo(tempo);
    auto at = tracks;

    for (int i = 0; i < 64; i++)
    {
        auto t = mpcSeq->getTrack(i);
        t->setUsed(at->getStatus(i) == 5 || at->getStatus(i) == 7);
        t->setName(at->getName(i));
        t->setDeviceIndex(at->getDevice(i));
        t->setBusNumber(at->getBus(i));
        t->setProgramChange(at->getPgm(i));
        t->setOn(at->getStatus(i) == 6 || at->getStatus(i) == 7);
        t->setVelocityRatio(at->getVelo(i));
    }

    for (int j = 0; j < getEventAmount(); j++)
    {
        auto e = allEvents[j];
        if (e == nullptr) continue;
        int track = e->getTrack();
        if (track > 128) track -= 128;
        if (track < 0) track += 128;
        if (track > 63) track -= 64;
        mpcSeq->getTrack(track)->cloneEventIntoTrack(e, e->getTick());
    }

    for (int i = 0; i < 32; i++)
        mpcSeq->setDeviceName(i, devNames[i]);

    mpcSeq->setFirstLoopBarIndex(loopFirst);
    mpcSeq->setLastLoopBarIndex(loopLast);
    mpcSeq->setLastLoopBarIndex(loopLast);

    if (loopLastEnd)
        mpcSeq->setLastLoopBarIndex(INT_MAX);

    mpcSeq->setLoopEnabled(loop);
}


AllSequence::AllSequence(mpc::sequencer::Sequence* seq, int number)
{
    auto segmentCountLastEventIndex = SequenceNames::getSegmentCount(seq);
    auto segmentCount = getSegmentCount(seq);
    auto terminatorCount = (segmentCount & 1) == 0 ? 2 : 1;
    saveBytes = std::vector<char>(10240 + (segmentCount * AllSequence::EVENT_SEG_LENGTH) + (terminatorCount * AllSequence::EVENT_SEG_LENGTH));
    
    for (int i = 0; i < AllParser::NAME_LENGTH; i++)
        saveBytes[i] = StrUtil::padRight(seq->getName(), " ", AllParser::NAME_LENGTH)[i];
    
    if ((segmentCountLastEventIndex & 1) != 0)
        segmentCountLastEventIndex--;
    
    segmentCountLastEventIndex /= 2;
    auto lastEventIndexBytes = ByteUtil::ushort2bytes(1 + (segmentCountLastEventIndex < 0 ? 0 : segmentCountLastEventIndex));

    saveBytes[LAST_EVENT_INDEX_OFFSET] = lastEventIndexBytes[0];
    saveBytes[LAST_EVENT_INDEX_OFFSET + 1] = lastEventIndexBytes[1];
    
    for (int i = AllSequence::PADDING1_OFFSET; i < PADDING1_OFFSET + PADDING1.size(); i++)
        saveBytes[i] = PADDING1[i - PADDING1_OFFSET];
    
    setTempoDouble(seq->getInitialTempo());
    
    for (int i = AllSequence::PADDING2_OFFSET; i < PADDING2_OFFSET + PADDING2.size(); i++)
        saveBytes[i] = PADDING2[i - PADDING2_OFFSET];
    
    setBarCount(seq->getLastBarIndex() + 1);
    setLastTick(seq);
    saveBytes[SEQUENCE_INDEX_OFFSET] = (number);
    setUnknown32BitInt(seq);
    auto loopStartBytes = ByteUtil::ushort2bytes(seq->getFirstLoopBarIndex());
    auto loopEndBytes = ByteUtil::ushort2bytes(seq->getLastLoopBarIndex());
    
    if (seq->isLastLoopBarEnd())
    {
        loopEndBytes = std::vector<char>{ (char)255, (char)255 };
    }
    
    saveBytes[LOOP_FIRST_OFFSET] = loopStartBytes[0];
    saveBytes[LOOP_FIRST_OFFSET + 1] = loopStartBytes[1];
    saveBytes[LOOP_LAST_OFFSET] = loopEndBytes[0];
    saveBytes[LOOP_LAST_OFFSET + 1] = loopEndBytes[1];
    saveBytes[LOOP_ENABLED_OFFSET] = seq->isLoopEnabled() ? 1 : 0;
    
    for (int i = 0; i < PADDING4.size(); i++)
    {
        saveBytes[PADDING4_OFFSET + i] = PADDING4[i];
    }
    
    for (int i = 0; i < 33; i++)
    {
        auto offset = DEVICE_NAMES_OFFSET + (i * AllParser::DEV_NAME_LENGTH);
        
        for (int j = 0; j < AllParser::DEV_NAME_LENGTH; j++)
        {
            saveBytes[offset + j] = StrUtil::padRight(seq->getDeviceName(i), " ", AllParser::DEV_NAME_LENGTH)[j];
        }
    }
    Tracks allFileSeqTracks(seq);
    
    for (int i = 0; i < TRACKS_LENGTH; i++)
    {
        saveBytes[i + TRACKS_OFFSET] = allFileSeqTracks.getBytes()[i];
    }
    
    BarList allFileBarList(seq);
    
    for (int i = AllSequence::BAR_LIST_OFFSET; i < BAR_LIST_OFFSET + BAR_LIST_LENGTH; i++)
    {
        saveBytes[i] = allFileBarList.getBytes()[i - BAR_LIST_OFFSET];
    }
    
    auto eventArraysChunk = createEventSegmentsChunk(seq);
    
    for (int i = AllSequence::EVENTS_OFFSET; i < EVENTS_OFFSET + eventArraysChunk.size(); i++)
    {
        saveBytes[i] = eventArraysChunk[i - EVENTS_OFFSET];
    }
    
    for (int i = (int)(saveBytes.size()) - 8; i < saveBytes.size(); i++)
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
std::vector<char> AllSequence::TERMINATOR = std::vector<char>{ (char) 0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF };
const int AllSequence::MAX_EVENT_SEG_COUNT;
const int AllSequence::EVENT_SEG_LENGTH;
const int AllSequence::NAME_OFFSET;
const int AllSequence::LAST_EVENT_INDEX_OFFSET;
const int AllSequence::SEQUENCE_INDEX_OFFSET;
const int AllSequence::PADDING1_OFFSET;
std::vector<char> AllSequence::PADDING1 = std::vector<char>{ 1, 1, 0 };
const int AllSequence::TEMPO_BYTE1_OFFSET;
const int AllSequence::TEMPO_BYTE2_OFFSET;
const int AllSequence::PADDING2_OFFSET;
std::vector<char> AllSequence::PADDING2 = std::vector<char>{ 4, 4 };
const int AllSequence::BAR_COUNT_BYTE1_OFFSET;
const int AllSequence::BAR_COUNT_BYTE2_OFFSET;
const int AllSequence::LAST_TICK_BYTE1_OFFSET;
const int AllSequence::LAST_TICK_BYTE2_OFFSET;
const int AllSequence::UNKNOWN32_BIT_INT_OFFSET;
const int AllSequence::LOOP_FIRST_OFFSET;
const int AllSequence::LOOP_LAST_OFFSET;
const int AllSequence::LOOP_ENABLED_OFFSET;
const int AllSequence::PADDING4_OFFSET;
std::vector<char> AllSequence::PADDING4 = std::vector<char>{ 40, 0, (char) 128, 0, 0 };
const int AllSequence::LAST_TICK_BYTE3_OFFSET;
const int AllSequence::LAST_TICK_BYTE4_OFFSET;
const int AllSequence::DEVICE_NAMES_OFFSET;
const int AllSequence::TRACKS_OFFSET;
const int AllSequence::TRACKS_LENGTH;
const int AllSequence::BAR_LIST_OFFSET;
const int AllSequence::BAR_LIST_LENGTH;
const int AllSequence::EVENTS_OFFSET;

std::vector<std::shared_ptr<mpc::sequencer::Event>> AllSequence::readEvents(const std::vector<char>& sequenceBytes)
{
    std::vector<std::shared_ptr<Event>> events;
    
    for (auto& eventBytes : readEventSegments(sequenceBytes))
        events.push_back(AllEvent::bytesToMpcEvent(eventBytes));
    
    return events;
}

std::vector<std::vector<char>> AllSequence::readEventSegments(const std::vector<char>& seqBytes)
{
    std::vector<std::vector<char>> eventArrays;
    int candidateOffset = AllSequence::EVENTS_OFFSET;
    
    for (int i = 0; i < MAX_EVENT_SEG_COUNT; i++)
    {
        auto ea = Util::vecCopyOfRange(seqBytes, candidateOffset, candidateOffset + EVENT_SEG_LENGTH);
        
        if (Util::vecEquals(ea, TERMINATOR))
            break;

        int sysexSegs;
        if (ea[EVENT_ID_OFFSET] == SYS_EX_ID) {
            for (sysexSegs = 0; sysexSegs < MAX_SYSEX_SIZE; sysexSegs++)
            {
                auto potentialTerminator = Util::vecCopyOfRange(seqBytes, candidateOffset + (sysexSegs * EVENT_SEG_LENGTH), candidateOffset + (sysexSegs * EVENT_SEG_LENGTH) + EVENT_SEG_LENGTH);
                
                if (potentialTerminator[EVENT_ID_OFFSET] == SYS_EX_TERMINATOR_ID)
                    break;
            }
            
            sysexSegs++;
            ea = Util::vecCopyOfRange(seqBytes, candidateOffset, candidateOffset + (sysexSegs * EVENT_SEG_LENGTH));
        }
        eventArrays.push_back(ea);
        candidateOffset += (int)(ea.size());
    }
    return eventArrays;
}

double AllSequence::getTempoDouble(const std::vector<char>& bytePair)
{
    auto s = ByteUtil::bytes2ushort(bytePair);
    return static_cast<double>(s) / 10.0;
}

int AllSequence::getNumberOfEventSegmentsForThisSeq(const std::vector<char>& seqBytes)
{
    auto accum = 0;
    
    for (auto& ba : readEventSegments(seqBytes))
        accum += (int) (ba.size()) / 8;
    
    return accum;
}

int AllSequence::getEventAmount()
{
    return (int)(allEvents.size());
}

int AllSequence::getSegmentCount(mpc::sequencer::Sequence* seq)
{
    int segmentCount = 0;
    
    for (auto& track : seq->getTracks())
    {
        if (track->getIndex() > 63)
            break;
        
        for (auto& e : track->getEvents())
        {
            if (const auto sysExEvent = std::dynamic_pointer_cast<mpc::sequencer::SystemExclusiveEvent>(e); sysExEvent)
            {
                int dataSegments = (int)(ceil((int)(sysExEvent->getBytes().size()) / 8.0));
                segmentCount += dataSegments + 2;
            }
            else if (const auto mixerEvent = std::dynamic_pointer_cast<mpc::sequencer::MixerEvent>(e); mixerEvent)
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

void AllSequence::setUnknown32BitInt(mpc::sequencer::Sequence* seq)
{
    auto unknownNumberBytes1 = ByteUtil::uint2bytes(10000000);
    auto unknownNumberBytes2 = ByteUtil::uint2bytes(seq->getLastTick() * 5208.333333333333);
    
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 4; j++)
            saveBytes[UNKNOWN32_BIT_INT_OFFSET + j + (i * 4)] = unknownNumberBytes1[j];
    }
    
    for (int i = 2; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
            saveBytes[UNKNOWN32_BIT_INT_OFFSET + j + (i * 4)] = unknownNumberBytes2[j];
    }
}

void AllSequence::setBarCount(int i)
{
    auto ba = ByteUtil::ushort2bytes(i);
    saveBytes[BAR_COUNT_BYTE1_OFFSET] = ba[0];
    saveBytes[BAR_COUNT_BYTE2_OFFSET] = ba[1];
}

std::vector<char> AllSequence::createEventSegmentsChunk(mpc::sequencer::Sequence* seq)
{
    std::vector<std::vector<char>> ea;
    
    for (int i = 0; i < seq->getLastTick(); i++)
    {
        for (auto& track : seq->getTracks())
        {
            if (track->getIndex() > 63)
                break;
            
            for (auto& event : track->getEvents())
            {
                if (event->getTick() == i)
                {
                    event->setTrack(track->getIndex());
                    ea.push_back(AllEvent::mpcEventToBytes(event));
                }
            }
        }
    }
    
    ea.push_back(TERMINATOR);
    return ByteUtil::stitchByteArrays(ea);
}

void AllSequence::setTempoDouble(double tempoForSaveBytes)
{
    auto ba = ByteUtil::ushort2bytes((unsigned short)(tempoForSaveBytes * 10.0));
    saveBytes[TEMPO_BYTE1_OFFSET] = ba[0];
    saveBytes[TEMPO_BYTE2_OFFSET] = ba[1];
}

void AllSequence::setLastTick(mpc::sequencer::Sequence* seq)
{
    auto lastTick = static_cast< int >(seq->getLastTick());
    auto remainder = lastTick % 65536;
    auto b = ByteUtil::ushort2bytes(remainder);
    auto large = static_cast< int >(floor(lastTick / 65536.0));
    saveBytes[LAST_TICK_BYTE1_OFFSET] = b[0];
    saveBytes[LAST_TICK_BYTE2_OFFSET] = b[1];
    saveBytes[LAST_TICK_BYTE2_OFFSET + 1] = (large);
    saveBytes[LAST_TICK_BYTE3_OFFSET] = b[0];
    saveBytes[LAST_TICK_BYTE4_OFFSET] = b[1];
    saveBytes[LAST_TICK_BYTE4_OFFSET + 1] = (large);
}

std::vector<char>& AllSequence::getBytes()
{
    return saveBytes;
}
