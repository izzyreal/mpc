#include "AllParser.hpp"

#include <stdexcept>

#include <Mpc.hpp>

#include <file/all/Count.hpp>
#include <file/all/Defaults.hpp>
#include <file/all/Header.hpp>
#include <file/all/MidiInput.hpp>
#include <file/all/MidiSyncMisc.hpp>
#include <file/all/Misc.hpp>
#include <file/all/AllSequence.hpp>
#include <file/all/SequenceNames.hpp>
#include <file/all/AllSequencer.hpp>
#include <file/all/AllSong.hpp>

#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>

#include <file/ByteUtil.hpp>

#include <Util.hpp>

using namespace mpc::file::all;

AllParser::AllParser(mpc::Mpc& _mpc, const std::vector<char>& loadBytes)
	: mpc (_mpc)
{
  if (loadBytes.size() >= HEADER_OFFSET + HEADER_LENGTH)
    header = new Header(Util::vecCopyOfRange(loadBytes, HEADER_OFFSET, HEADER_OFFSET + HEADER_LENGTH));
	
	if (header == nullptr || !header->verifyFileID())
		throw std::invalid_argument("Invalid ALL file header ID");
	
	defaults = new Defaults(mpc, Util::vecCopyOfRange(loadBytes, DEFAULTS_OFFSET, DEFAULTS_OFFSET + DEFAULTS_LENGTH));
	sequencer = new AllSequencer(Util::vecCopyOfRange(loadBytes, SEQUENCER_OFFSET, SEQUENCER_OFFSET + AllSequencer::LENGTH));
	count = new Count(Util::vecCopyOfRange(loadBytes, COUNT_OFFSET, COUNT_OFFSET + COUNT_LENGTH));
	midiInput = new MidiInput(Util::vecCopyOfRange(loadBytes, MIDI_INPUT_OFFSET, MIDI_INPUT_OFFSET + MidiInput::LENGTH));
	midiSyncMisc = new MidiSyncMisc(Util::vecCopyOfRange(loadBytes, MIDI_SYNC_OFFSET, MIDI_SYNC_OFFSET + MidiSyncMisc::LENGTH));
	misc = new Misc(Util::vecCopyOfRange(loadBytes, MISC_OFFSET, MISC_OFFSET + Misc::LENGTH));
	seqNames = new SequenceNames(Util::vecCopyOfRange(loadBytes, SEQUENCE_NAMES_OFFSET, SEQUENCE_NAMES_OFFSET + SequenceNames::LENGTH));
	
	for (int i = 0; i < 20; i++)
	{
		int offset = SONGS_OFFSET + (i * Song::LENGTH);
		songs[i] = new Song(Util::vecCopyOfRange(loadBytes, offset, offset + Song::LENGTH));
	}
	
	sequences = readSequences(Util::vecCopyOfRange(loadBytes, SEQUENCES_OFFSET, loadBytes.size()));
}

AllParser::AllParser(mpc::Mpc& _mpc)
	: mpc (_mpc)
{
	std::vector<std::vector<char>> chunks;
	chunks.push_back(Header().getBytes());
	
	Defaults saveDefaults(mpc);
	
	chunks.push_back(saveDefaults.getBytes());
	chunks.push_back(UNKNOWN_CHUNK);
	sequencer = new AllSequencer(mpc);
	chunks.push_back(sequencer->getBytes());
	count = new Count(mpc);
	chunks.push_back(count->getBytes());
	midiInput = new MidiInput(mpc);
	chunks.push_back(midiInput->getBytes());

	for (int i = 0; i < 16; i++)
		chunks.push_back(std::vector<char>{ (char)0xFF });

	midiSyncMisc = new MidiSyncMisc(mpc);
	chunks.push_back(midiSyncMisc->getBytes());
	misc = new Misc(mpc);
	chunks.push_back(misc->getBytes());
	seqNames = new SequenceNames(mpc);
	chunks.push_back(seqNames->getBytes());

	auto mpcSequencer = mpc.getSequencer();

	for (int i = 0; i < 20; i++) {
		songs[i] = new Song(mpcSequencer->getSong(i).get());
		chunks.push_back(songs[i]->getBytes());
	}
	
	auto usedSeqs = mpcSequencer->getUsedSequences();
	
	for (int i = 0; i < usedSeqs.size(); i++)
	{
		auto seq = usedSeqs[i];
		AllSequence allSeq(seq.get(), mpcSequencer->getUsedSequenceIndexes()[i] + 1);
		chunks.push_back(allSeq.getBytes());
	}
	
	saveBytes = ByteUtil::stitchByteArrays(chunks);
}

AllParser::~AllParser()
{
	if (header != nullptr) delete header;
	if (defaults != nullptr) delete defaults;
	if (sequencer != nullptr) delete sequencer;
	if (count != nullptr) delete count;
	if (midiInput != nullptr) delete midiInput;
	if (midiSyncMisc != nullptr) delete midiSyncMisc;
	if (misc != nullptr) delete misc;
	if (seqNames != nullptr) delete seqNames;

	for (auto& s : sequences)
		if (s != nullptr)
            delete s;
	
	for (auto& s : songs)
		if (s != nullptr) delete s;
}

std::vector<AllSequence*> AllParser::getAllSequences()
{
    return sequences;
}

Defaults* AllParser::getDefaults()
{
    return defaults;
}

mpc::file::all::AllSequencer* AllParser::getSequencer()
{
	return sequencer;
}

Count* AllParser::getCount()
{
    return count;
}

MidiInput* AllParser::getMidiInput()
{
    return midiInput;
}

MidiSyncMisc* AllParser::getMidiSync()
{
    return midiSyncMisc;
}

Misc* AllParser::getMisc()
{
    return misc;
}

SequenceNames* AllParser::getSeqNames()
{
    return seqNames;
}

std::vector<mpc::file::all::Song*> AllParser::getSongs()
{
	return songs;
}

std::vector<AllSequence*> AllParser::readSequences(std::vector<char> trimmedSeqsArray)
{
	const int totalSeqChunkLength = trimmedSeqsArray.size();
    
    if (totalSeqChunkLength == 0)
        return {};
    
	std::vector<AllSequence*> seqs;
	int eventSegments, currentSeqEnd, read = 0;

    auto usednesses = seqNames->getUsednesses();

	for (const auto& u : usednesses)
    {
        if (!u)
        {
            seqs.push_back(nullptr);
            continue;
        }

		eventSegments = AllSequence::getNumberOfEventSegmentsForThisSeq(trimmedSeqsArray);
		currentSeqEnd = EMPTY_SEQ_LENGTH + (eventSegments * EVENT_LENGTH);
	
        if (currentSeqEnd > trimmedSeqsArray.size())
            currentSeqEnd -= 8;
		
        auto currentSeqArray = Util::vecCopyOfRange(trimmedSeqsArray, 0, currentSeqEnd);
		auto as = new AllSequence(currentSeqArray);
		seqs.push_back(as);
		read += currentSeqEnd;
		int multiplier = (eventSegments & 1) == 0 ? 0 : 1;
		
        if (totalSeqChunkLength - read >= EMPTY_SEQ_LENGTH - 16)
        {
			trimmedSeqsArray = Util::vecCopyOfRange(trimmedSeqsArray, currentSeqEnd - (multiplier * EVENT_LENGTH), trimmedSeqsArray.size());
		}
		else
        {
			break;
		}
	}
    
	return seqs;
}

std::vector<char>& AllParser::getBytes()
{
    return saveBytes;
}

std::vector<char> AllParser::UNKNOWN_CHUNK = std::vector<char>(64);
