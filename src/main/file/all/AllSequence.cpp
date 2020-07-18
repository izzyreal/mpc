#include <file/all/AllSequence.hpp>

#include <Util.hpp>
#include <file/all/AllEvent.hpp>
#include <file/all/AllParser.hpp>
#include <file/all/BarList.hpp>
#include <file/all/SequenceNames.hpp>
#include <file/all/Tracks.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/SystemExclusiveEvent.hpp>
#include <sequencer/TempoChangeEvent.hpp>

#include <file/ByteUtil.hpp>
#include <lang/StrUtil.hpp>
#include <VecUtil.hpp>

#include <cmath>

using namespace std;
using namespace moduru::lang;
using namespace mpc::file::all;

Sequence::Sequence(vector<char> b) 
{
	barList = new BarList(moduru::VecUtil::CopyOfRange(&b, BAR_LIST_OFFSET, BAR_LIST_OFFSET + BAR_LIST_LENGTH));
	auto nameBytes = moduru::VecUtil::CopyOfRange(&b, NAME_OFFSET, NAME_OFFSET + AllParser::NAME_LENGTH);
	name = "";
	
	for (char c : nameBytes)
	{
		if (c == 0x00)
		{
			break;
		}
		name.push_back(c);
	}

	tempo = getTempoDouble(vector<char>{ b[TEMPO_BYTE1_OFFSET] , b[TEMPO_BYTE2_OFFSET] });
	auto barCountBytes = vector<char>{ b[BAR_COUNT_BYTE1_OFFSET], b[BAR_COUNT_BYTE2_OFFSET] };
	barCount = moduru::file::ByteUtil::bytes2ushort(barCountBytes);
	loopFirst = moduru::file::ByteUtil::bytes2ushort(vector<char>{ b[LOOP_FIRST_OFFSET], b[LOOP_FIRST_OFFSET + 1] });
	loopLast = moduru::file::ByteUtil::bytes2ushort(vector<char>{ b[LOOP_LAST_OFFSET], b[LOOP_LAST_OFFSET + 1] });

	if (loopLast > 998)
	{
		loopLast = barCount;
		loopLastEnd = true;
	}
	
	loop = (b[LOOP_ENABLED_OFFSET] > 0);
	
	for (int i = 0; i < 33; i++)
	{
		auto offset = DEVICE_NAMES_OFFSET + (i * AllParser::DEV_NAME_LENGTH);
		string stringBuffer = "";
		auto stringBytes = moduru::VecUtil::CopyOfRange(&b, offset, offset + AllParser::DEV_NAME_LENGTH);

		for (char c : stringBytes)
		{
			if (c == 0x00)
			{
				break;
			}
			stringBuffer.push_back(c);
		}
		devNames[i] = stringBuffer;
	}
	tracks = new Tracks(moduru::VecUtil::CopyOfRange(&b, TRACKS_OFFSET, TRACKS_LENGTH));
	allEvents = readEvents(b);
}

Sequence::Sequence(mpc::sequencer::Sequence* seq, int number)
{
	auto segmentCountLastEventIndex = SequenceNames::getSegmentCount(seq);
	auto segmentCount = getSegmentCount(seq);
	auto terminatorCount = (segmentCount & 1) == 0 ? 2 : 1;
	saveBytes = vector<char>(10240 + (segmentCount * Sequence::EVENT_SEG_LENGTH) + (terminatorCount * Sequence::EVENT_SEG_LENGTH));

	for (int i = 0; i < AllParser::NAME_LENGTH; i++)
	{
		saveBytes[i] = StrUtil::padRight(seq->getName(), " ", AllParser::NAME_LENGTH)[i];
	}

	if ((segmentCountLastEventIndex & 1) != 0)
	{
		segmentCountLastEventIndex--;
	}

	segmentCountLastEventIndex /= 2;
	auto lastEventIndexBytes = moduru::file::ByteUtil::ushort2bytes(1 + (segmentCountLastEventIndex < 0 ? 0 : segmentCountLastEventIndex));
	saveBytes[LAST_EVENT_INDEX_OFFSET] = lastEventIndexBytes[0];
	saveBytes[LAST_EVENT_INDEX_OFFSET + 1] = lastEventIndexBytes[1];
	
	for (int i = Sequence::PADDING1_OFFSET; i < PADDING1_OFFSET + PADDING1.size(); i++)
	{
		saveBytes[i] = PADDING1[i - PADDING1_OFFSET];
	}

	setTempoDouble(seq->getInitialTempo());
	
	for (int i = Sequence::PADDING2_OFFSET; i < PADDING2_OFFSET + PADDING2.size(); i++)
	{
		saveBytes[i] = PADDING2[i - PADDING2_OFFSET];
	}

	setBarCount(seq->getLastBarIndex() + 1);
	setLastTick(seq);
	saveBytes[SEQUENCE_INDEX_OFFSET] = (number);
	setUnknown32BitInt(seq);
	auto loopStartBytes = moduru::file::ByteUtil::ushort2bytes(seq->getFirstLoopBarIndex());
	auto loopEndBytes = moduru::file::ByteUtil::ushort2bytes(seq->getLastLoopBar());
	
	if (seq->isLastLoopBarEnd())
	{
		loopEndBytes = vector<char>{ (char)255, (char)255 };
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
	auto tracks = Tracks(seq);

	for (int i = 0; i < TRACKS_LENGTH; i++)
	{
		saveBytes[i + TRACKS_OFFSET] = tracks.getBytes()[i];
	}

	auto barList = BarList(seq);

	for (int i = Sequence::BAR_LIST_OFFSET; i < BAR_LIST_OFFSET + BAR_LIST_LENGTH; i++)
	{
		saveBytes[i] = barList.getBytes()[i - BAR_LIST_OFFSET];
	}

	auto eventArraysChunk = createEventSegmentsChunk(seq);

	for (int i = Sequence::EVENTS_OFFSET; i < EVENTS_OFFSET + eventArraysChunk.size(); i++)
	{
		saveBytes[i] = eventArraysChunk[i - EVENTS_OFFSET];
	}

	for (int i = (int)(saveBytes.size()) - 8; i < saveBytes.size(); i++)
	{
		saveBytes[i] = (255);
	}
}

const int Sequence::MAX_SYSEX_SIZE;
const int Sequence::EVENT_ID_OFFSET;
const char Sequence::POLY_PRESSURE_ID;
const char Sequence::CONTROL_CHANGE_ID;
const char Sequence::PGM_CHANGE_ID;
const char Sequence::CH_PRESSURE_ID;
const char Sequence::PITCH_BEND_ID;
const char Sequence::SYS_EX_ID;
const char Sequence::SYS_EX_TERMINATOR_ID;
vector<char> Sequence::TERMINATOR = vector<char>{ (char) 0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF };
const int Sequence::MAX_EVENT_SEG_COUNT;
const int Sequence::EVENT_SEG_LENGTH;
const int Sequence::NAME_OFFSET;
const int Sequence::LAST_EVENT_INDEX_OFFSET;
const int Sequence::SEQUENCE_INDEX_OFFSET;
const int Sequence::PADDING1_OFFSET;
vector<char> Sequence::PADDING1 = vector<char>{ 1, 1, 0 };
const int Sequence::TEMPO_BYTE1_OFFSET;
const int Sequence::TEMPO_BYTE2_OFFSET;
const int Sequence::PADDING2_OFFSET;
vector<char> Sequence::PADDING2 = vector<char>{ 4, 4 };
const int Sequence::BAR_COUNT_BYTE1_OFFSET;
const int Sequence::BAR_COUNT_BYTE2_OFFSET;
const int Sequence::LAST_TICK_BYTE1_OFFSET;
const int Sequence::LAST_TICK_BYTE2_OFFSET;
const int Sequence::UNKNOWN32_BIT_INT_OFFSET;
const int Sequence::LOOP_FIRST_OFFSET;
const int Sequence::LOOP_LAST_OFFSET;
const int Sequence::LOOP_ENABLED_OFFSET;
const int Sequence::PADDING4_OFFSET;
vector<char> Sequence::PADDING4 = vector<char>{ 40, 0, (char) 128, 0, 0 };
const int Sequence::LAST_TICK_BYTE3_OFFSET;
const int Sequence::LAST_TICK_BYTE4_OFFSET;
const int Sequence::DEVICE_NAMES_OFFSET;
const int Sequence::TRACKS_OFFSET;
const int Sequence::TRACKS_LENGTH;
const int Sequence::BAR_LIST_OFFSET;
const int Sequence::BAR_LIST_LENGTH;
const int Sequence::EVENTS_OFFSET;

vector<mpc::sequencer::Event*> Sequence::readEvents(vector<char> seqBytes)
{
	vector<mpc::sequencer::Event*> aeList;
	for (auto& ba : readEventSegments(seqBytes)) {
		auto ae = AllEvent(ba);
		aeList.push_back(ae.getEvent());
	}
	return aeList;
}

vector<vector<char>> Sequence::readEventSegments(vector<char> seqBytes)
{
	vector<vector<char>> eventArrays;
	int candidateOffset = Sequence::EVENTS_OFFSET;
	for (int i = 0; i < MAX_EVENT_SEG_COUNT; i++) {
		int sysexSegs = 0;
		auto ea = moduru::VecUtil::CopyOfRange(&seqBytes, candidateOffset, candidateOffset + EVENT_SEG_LENGTH);
		if (moduru::VecUtil::Equals(ea, TERMINATOR)) break;

		if (ea[EVENT_ID_OFFSET] == SYS_EX_ID) {
			for (sysexSegs = 0; sysexSegs < MAX_SYSEX_SIZE; sysexSegs++) {
				auto potentialTerminator = moduru::VecUtil::CopyOfRange(&seqBytes, candidateOffset + (sysexSegs * EVENT_SEG_LENGTH), candidateOffset + (sysexSegs * EVENT_SEG_LENGTH) + EVENT_SEG_LENGTH);
				if (potentialTerminator[EVENT_ID_OFFSET] == SYS_EX_TERMINATOR_ID) break;
			}
			sysexSegs++;
			ea = moduru::VecUtil::CopyOfRange(&seqBytes, candidateOffset, candidateOffset + (sysexSegs * EVENT_SEG_LENGTH));
		}
		eventArrays.push_back(ea);
		candidateOffset += (int)(ea.size());
	}
	return eventArrays;
}

double Sequence::getTempoDouble(vector<char> bytePair)
{
	double k = 0;
	auto s = moduru::file::ByteUtil::bytes2ushort(bytePair);
	k = static_cast<double>(s);
	return k / 10.0;
}

int Sequence::getNumberOfEventSegmentsForThisSeq(vector<char> seqBytes)
{
	auto accum = 0;
	for (auto& ba : readEventSegments(seqBytes)) {
		accum += (int) (ba.size()) / 8;
	}
	return accum;
}

int Sequence::getEventAmount()
{
	return (int)(allEvents.size());
}

int Sequence::getSegmentCount(mpc::sequencer::Sequence* seq)
{
	int segmentCount = 0;
	
	for (auto& track : seq->getTracks())
	{
		auto t = track.lock();
		
		if (t->getTrackIndex() > 63)
		{
			break;
		}

		for (auto& e : t->getEvents())
		{
			auto sysExEvent = dynamic_pointer_cast<mpc::sequencer::SystemExclusiveEvent>(e.lock());
			auto mixerEvent = dynamic_pointer_cast<mpc::sequencer::MixerEvent>(e.lock());
			
			if (sysExEvent)
			{
				int dataSegments = (int)(ceil((int)(sysExEvent->getBytes().size()) / 8.0));
				segmentCount += dataSegments + 2;
			}
			else if (mixerEvent)
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

void Sequence::setUnknown32BitInt(mpc::sequencer::Sequence* seq)
{
	auto unknownNumberBytes1 = moduru::file::ByteUtil::uint2bytes(10000000);
	auto unknownNumberBytes2 = moduru::file::ByteUtil::uint2bytes(seq->getLastTick() * 5208.333333333333);
	
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			saveBytes[UNKNOWN32_BIT_INT_OFFSET + j + (i * 4)] = unknownNumberBytes1[j];
		}
	}

	for (int i = 2; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			saveBytes[UNKNOWN32_BIT_INT_OFFSET + j + (i * 4)] = unknownNumberBytes2[j];
		}
	}
}

void Sequence::setBarCount(int i)
{
    auto ba = moduru::file::ByteUtil::ushort2bytes(i);
    saveBytes[BAR_COUNT_BYTE1_OFFSET] = ba[0];
    saveBytes[BAR_COUNT_BYTE2_OFFSET] = ba[1];
}

vector<char> Sequence::createEventSegmentsChunk(mpc::sequencer::Sequence* seq)
{
	vector<vector<char>> ea;

	for (int i = 0; i < seq->getLastTick(); i++)
	{
		for (auto& track : seq->getTracks())
		{
			auto t = track.lock();

			if (t->getTrackIndex() > 63)
			{
				break;
			}

			for (auto& event : t->getEvents())
			{
				auto e = event.lock();

				if (e->getTick() == i)
				{
					e->setTrack(t->getTrackIndex());
					auto ae = AllEvent(e.get());
					ea.push_back(ae.getBytes());
				}
			}
		}
	}
	ea.push_back(TERMINATOR);
	return moduru::file::ByteUtil::stitchByteArrays(ea);
}

void Sequence::setTempoDouble(double tempo)
{
	auto ba = moduru::file::ByteUtil::ushort2bytes((unsigned short)(tempo * 10.0));
	saveBytes[TEMPO_BYTE1_OFFSET] = ba[0];
	saveBytes[TEMPO_BYTE2_OFFSET] = ba[1];
}

void Sequence::setLastTick(mpc::sequencer::Sequence* seq)
{
    auto lastTick = static_cast< int >(seq->getLastTick());
    auto remainder = lastTick % 65536;
    auto b = moduru::file::ByteUtil::ushort2bytes(remainder);
    auto large = static_cast< int >(floor(lastTick / 65536.0));
    saveBytes[LAST_TICK_BYTE1_OFFSET] = b[0];
    saveBytes[LAST_TICK_BYTE2_OFFSET] = b[1];
    saveBytes[LAST_TICK_BYTE2_OFFSET + 1] = (large);
    saveBytes[LAST_TICK_BYTE3_OFFSET] = b[0];
    saveBytes[LAST_TICK_BYTE4_OFFSET] = b[1];
    saveBytes[LAST_TICK_BYTE4_OFFSET + 1] = (large);
}

vector<char> Sequence::getBytes()
{
    return saveBytes;
}
