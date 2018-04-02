#include <file/all/AllParser.hpp>

#include <Mpc.hpp>
#include <Util.hpp>
#include <disk/MpcFile.hpp>
//#include <file/Definitions.hpp>
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
#include <StartUp.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>

#include <file/ByteUtil.hpp>
#include <VecUtil.hpp>

using namespace mpc::file::all;
using namespace std;

AllParser::AllParser(mpc::disk::MpcFile* file)
{
	songs = vector<Song*>(20);
	auto loadBytes = file->getBytes();
	header = new Header(moduru::VecUtil::CopyOfRange(&loadBytes, HEADER_OFFSET, HEADER_OFFSET + HEADER_LENGTH));
	header->verifyFileID();
	defaults = new Defaults(moduru::VecUtil::CopyOfRange(&loadBytes, DEFAULTS_OFFSET, DEFAULTS_OFFSET + DEFAULTS_LENGTH));
	sequencer = new Sequencer(moduru::VecUtil::CopyOfRange(&loadBytes, SEQUENCER_OFFSET, SEQUENCER_OFFSET + Sequencer::LENGTH));
	count = new Count(moduru::VecUtil::CopyOfRange(&loadBytes, COUNT_OFFSET, COUNT_OFFSET + COUNT_LENGTH));
	midiInput = new MidiInput(moduru::VecUtil::CopyOfRange(&loadBytes, MIDI_INPUT_OFFSET, MIDI_INPUT_OFFSET + MidiInput::LENGTH));
	midiSyncMisc = new MidiSyncMisc(moduru::VecUtil::CopyOfRange(&loadBytes, MIDI_SYNC_OFFSET, MIDI_SYNC_OFFSET + MidiSyncMisc::LENGTH));
	misc = new Misc(moduru::VecUtil::CopyOfRange(&loadBytes, MISC_OFFSET, MISC_OFFSET + Misc::LENGTH));
	seqNames = new SequenceNames(moduru::VecUtil::CopyOfRange(&loadBytes, SEQUENCE_NAMES_OFFSET, SEQUENCE_NAMES_OFFSET + SequenceNames::LENGTH));
	for (int i = 0; i < 20; i++) {
		int offset = SONGS_OFFSET + (i * Song::LENGTH);
		songs[i] = new Song(moduru::VecUtil::CopyOfRange(&loadBytes, offset, offset + Song::LENGTH));
	}
	sequences = readSequences(moduru::VecUtil::CopyOfRange(&loadBytes, SEQUENCES_OFFSET, loadBytes.size()));
}

AllParser::AllParser(mpc::Mpc* mpc, string allName) 
{
	songs = vector<Song*>(20);
	vector<vector<char>> chunks;
	auto header = new Header();
	chunks.push_back(header->getBytes());
	auto defaults = new Defaults(mpc::StartUp::getUserDefaults().lock().get());
	chunks.push_back(defaults->getBytes());
	chunks.push_back(UNKNOWN_CHUNK);
	sequencer = new Sequencer(mpc);
	chunks.push_back(sequencer->getBytes());
	count = new Count(mpc);
	chunks.push_back(count->getBytes());
	midiInput = new MidiInput(mpc);
	chunks.push_back(midiInput->getBytes());
	
	for (int i=0;i<16;i++)
		chunks.push_back(vector<char>{ (char) 0xFF });

	midiSyncMisc = new MidiSyncMisc(mpc);
	chunks.push_back(midiSyncMisc->getBytes());
	misc = new Misc(mpc);
	chunks.push_back(misc->getBytes());
	seqNames = new SequenceNames(mpc);
	chunks.push_back(seqNames->getBytes());
	songs = vector<Song*>(20);
	auto sequencer = mpc->getSequencer().lock();
	for (int i = 0; i < 20; i++) {
		songs[i] = new Song(sequencer->getSong(i).lock().get());
		chunks.push_back(songs[i]->getBytes());
	}
	auto usedSeqs = sequencer->getUsedSequences();
	for (int i = 0; i < usedSeqs.size(); i++) {
		auto seq = usedSeqs[i];
		auto allSeq = new Sequence(seq.lock().get(), sequencer->getUsedSequenceIndexes()[i] + 1);
		chunks.push_back(allSeq->getBytes());
	}
	saveBytes = moduru::file::ByteUtil::stitchByteArrays(chunks);
}

const int AllParser::NAME_LENGTH;
const int AllParser::DEV_NAME_LENGTH;
const int AllParser::EMPTY_SEQ_LENGTH;
const int AllParser::EVENT_LENGTH;
const int AllParser::HEADER_OFFSET;
const int AllParser::HEADER_LENGTH;
const int AllParser::DEFAULTS_OFFSET;
const int AllParser::DEFAULTS_LENGTH;
const int AllParser::UNKNOWN1_OFFSET;
const int AllParser::SEQUENCER_OFFSET;
const int AllParser::COUNT_OFFSET;
const int AllParser::COUNT_LENGTH;
const int AllParser::MIDI_INPUT_OFFSET;
const int AllParser::MIDI_SYNC_OFFSET;
const int AllParser::MISC_OFFSET;
const int AllParser::SEQUENCE_NAMES_OFFSET;
const int AllParser::SONGS_OFFSET;
const int AllParser::SEQUENCES_OFFSET;

vector<Sequence*> AllParser::getAllSequences()
{
    return sequences;
}

Defaults* AllParser::getDefaults()
{
    return defaults;
}

mpc::file::all::Sequencer* AllParser::getSequencer()
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

vector<mpc::file::all::Song*> AllParser::getSongs()
{
	return songs;
}

vector<Sequence*> AllParser::readSequences(vector<char> trimmedSeqsArray)
{
	const int totalSeqChunkLength = trimmedSeqsArray.size();
	vector<Sequence*> seqs;
	int eventSegments, currentSeqEnd, read = 0;
	for (int i = 0; i < 99; i++) {
		eventSegments = Sequence::getNumberOfEventSegmentsForThisSeq(trimmedSeqsArray);
		currentSeqEnd = EMPTY_SEQ_LENGTH + (eventSegments * EVENT_LENGTH);
		if (currentSeqEnd > trimmedSeqsArray.size()) currentSeqEnd -= 8;
		auto currentSeqArray = moduru::VecUtil::CopyOfRange(&trimmedSeqsArray, 0, currentSeqEnd);
		auto as = new Sequence(currentSeqArray);
		seqs.push_back(as);
		read += currentSeqEnd;
		int multiplier = (eventSegments & 1) == 0 ? 0 : 1;
		if (totalSeqChunkLength - read >= EMPTY_SEQ_LENGTH - 16) {
			trimmedSeqsArray = moduru::VecUtil::CopyOfRange(&trimmedSeqsArray, currentSeqEnd - (multiplier * EVENT_LENGTH), trimmedSeqsArray.size());
		}
		else {
			break;
		}
	}
	return seqs;
}

vector<char> AllParser::getBytes()
{
    return saveBytes;
}

AllParser::~AllParser() {
	/*
	if (header != nullptr) delete header;
	if (defaults != nullptr) delete defaults;
	if (sequencer != nullptr) delete sequencer;
	if (count != nullptr) delete count;
	if (midiInput != nullptr) delete midiInput;
	if (midiSyncMisc != nullptr) delete midiSyncMisc;
	if (misc != nullptr) delete misc;
	if (seqNames != nullptr) delete seqNames;
	for (auto s : sequences) {
		if (s != nullptr) delete s;
	}
	for (auto s : songs) {
		if (s != nullptr) delete s;
	}
	*/
}

vector<char> AllParser::UNKNOWN_CHUNK = vector<char>(64);
