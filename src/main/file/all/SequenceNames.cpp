#include <file/all/SequenceNames.hpp>

#include <Mpc.hpp>
#include <file/all/AllParser.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/SystemExclusiveEvent.hpp>

#include <file/ByteUtil.hpp>
#include <StrUtil.hpp>
#include <Util.hpp>

#include <cmath>

using namespace mpc::file::all;

SequenceNames::SequenceNames(const std::vector<char>& b)
{
	for (int i = 0; i < names.size(); i++)
	{	
		int offset = i * ENTRY_LENGTH;
		std::string stringBuffer = "";
		auto nameBytes = Util::vecCopyOfRange(b, offset, offset + AllParser::NAME_LENGTH);

		for (char c : nameBytes)
		{
			if (c == 0x00)
			{
				break;
			}
			stringBuffer.push_back(c);
		}
		names[i] = stringBuffer;

		auto usednessBytes = Util::vecCopyOfRange(b, offset + AllParser::NAME_LENGTH, offset + AllParser::NAME_LENGTH + 2);
		usednesses[i] = usednessBytes[0] != 0 || usednessBytes[1] != 0;
	}
}

SequenceNames::SequenceNames(mpc::Mpc& mpc)
{
	saveBytes = std::vector<char>(LENGTH);
	auto sequencer = mpc.getSequencer();

	for (int i = 0; i < 99; i++)
	{
		auto seq = sequencer->getSequence(i);
		auto name = seq->getName();
		auto offset = i * ENTRY_LENGTH;
		
		for (auto j = 0; j < AllParser::NAME_LENGTH; j++)
		{
			saveBytes[offset + j] = StrUtil::padRight(name, " ", 16)[j];
		}

		if (name.find("(Unused)") == std::string::npos)
		{
			auto eventSegmentCount = getSegmentCount(seq.get());
		
			if ((eventSegmentCount & 1) != 0)
			{
				eventSegmentCount--;
			}

			auto lastEventIndex = 641 + (eventSegmentCount / 2);
			
			if (lastEventIndex < 641)
			{
				lastEventIndex = 641;
			}

			auto eventCountBytes = ByteUtil::ushort2bytes(lastEventIndex);
			saveBytes[offset + LAST_EVENT_INDEX_OFFSET] = eventCountBytes[0];
			saveBytes[offset + LAST_EVENT_INDEX_OFFSET + 1] = eventCountBytes[1];
		}
	}
}

std::vector<std::string>& SequenceNames::getNames()
{
    return names;
}

std::vector<bool>& SequenceNames::getUsednesses()
{
    return usednesses;
}

std::vector<char>& SequenceNames::getBytes()
{
    return saveBytes;
}

int SequenceNames::getSegmentCount(mpc::sequencer::Sequence* seq)
{
	auto segmentCount = 0;
	for (auto& track : seq->getTracks())
	{

		if (track->getIndex() > 63)
			break;

		for (auto& e : track->getEvents())
		{
			auto sysEx = std::dynamic_pointer_cast<mpc::sequencer::SystemExclusiveEvent>(e);
		
			if (sysEx)
			{
				auto dataSegments = (int)(ceil(sysEx->getBytes().size() / 8.0));
				segmentCount += dataSegments + 1;
			}
			else if (std::dynamic_pointer_cast<mpc::sequencer::MixerEvent>(e))
			{
				segmentCount += 2;
			}
			else
			{
				segmentCount++;
			}
		}
	}
    
	return segmentCount;
}
