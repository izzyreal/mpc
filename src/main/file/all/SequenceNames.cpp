#include <file/all/SequenceNames.hpp>

#include <Mpc.hpp>
#include <Util.hpp>
#include <file/all/AllParser.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/SystemExclusiveEvent.hpp>

#include <file/ByteUtil.hpp>
#include <VecUtil.hpp>

#include <cmath>

using namespace std;
using namespace mpc::file::all;

SequenceNames::SequenceNames(vector<char> b)
{
	for (int i = 0; i < names.size(); i++) {
		int offset = i * ENTRY_LENGTH;
		string stringBuffer = "";
		auto nameBytes = moduru::VecUtil::CopyOfRange(&b, offset, offset + AllParser::NAME_LENGTH);
		for (char c : nameBytes) {
			if (c == 0x00) break;
			stringBuffer.push_back(c);
		}
		names[i] = stringBuffer;
	}
}

SequenceNames::SequenceNames()
{
	saveBytes = vector<char>(LENGTH);
	auto sequencer = Mpc::instance().getSequencer().lock();
	for (int i = 0; i < 99; i++) {
		auto seq = sequencer->getSequence(i).lock();
		auto name = seq->getName();
		auto offset = i * ENTRY_LENGTH;
		for (auto j = 0; j < AllParser::NAME_LENGTH; j++)
			saveBytes[offset + j] = moduru::lang::StrUtil::padRight(name, " ", 16)[j];

		if (name.find("(Unused)") == string::npos) {
			auto eventSegmentCount = getSegmentCount(seq.get());
			if ((eventSegmentCount & 1) != 0)
				eventSegmentCount--;

			auto lastEventIndex = 641 + (eventSegmentCount / 2);
			if (lastEventIndex < 641) lastEventIndex = 641;

			auto eventCountBytes = moduru::file::ByteUtil::ushort2bytes(lastEventIndex);
			saveBytes[offset + LAST_EVENT_INDEX_OFFSET] = eventCountBytes[0];
			saveBytes[offset + LAST_EVENT_INDEX_OFFSET + 1] = eventCountBytes[1];
		}
	}
}

const int SequenceNames::LENGTH;
const int SequenceNames::ENTRY_LENGTH;
const int SequenceNames::LAST_EVENT_INDEX_OFFSET;

vector<string> SequenceNames::getNames()
{
    return names;
}

vector<char> SequenceNames::getBytes()
{
    return saveBytes;
}

int SequenceNames::getSegmentCount(mpc::sequencer::Sequence* seq)
{
	auto segmentCount = 0;
	for (auto& track : seq->getTracks()) {
		auto t = track.lock();

		if (t->getTrackIndex() > 63) break;

		for (auto& e : t->getEvents()) {
			auto sysEx = dynamic_pointer_cast<mpc::sequencer::SystemExclusiveEvent>(e.lock());
			if (sysEx) {
				auto dataSegments = (int)(ceil(sysEx->getBytes()->size() / 8.0));
				segmentCount += dataSegments + 1;
			}
			else if (dynamic_pointer_cast<mpc::sequencer::MixerEvent>(e.lock())) {
				segmentCount += 2;
			}
			else {
				segmentCount++;
			}
		}
	}
	return segmentCount;
}
