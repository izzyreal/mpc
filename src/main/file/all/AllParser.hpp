#pragma once

#include <memory>
#include <vector>
#include <string>

namespace mpc { class Mpc; }

namespace mpc::disk {
	class MpcFile;
}

namespace mpc::file::all {
	class Header;
	class Defaults;
	class Sequencer;
	class Count;
	class MidiInput;
	class MidiSyncMisc;
	class Misc;
	class SequenceNames;
	class Sequence;
	class Song;
}

namespace mpc::file::all
{
	class AllParser
	{

	public:
		static const int NAME_LENGTH{ 16 };
		static const int DEV_NAME_LENGTH{ 8 };
		static const int EMPTY_SEQ_LENGTH{ 10256 };
		static const int EVENT_LENGTH{ 8 };
		static const int HEADER_OFFSET{ 0 };
		static const int HEADER_LENGTH{ 16 };
		static const int DEFAULTS_OFFSET{ 16 };
		static std::vector<char> UNKNOWN_CHUNK;

	public:
		static const int DEFAULTS_LENGTH{ 1728 };

	public:
		static const int UNKNOWN1_OFFSET{ 1744 };
		static const int SEQUENCER_OFFSET{ 1808 };
		static const int COUNT_OFFSET{ 1824 };
		static const int COUNT_LENGTH{ 16 };
		static const int MIDI_INPUT_OFFSET{ 1840 };
		static const int MIDI_SYNC_OFFSET{ 1904 };
		static const int MISC_OFFSET{ 1936 };
		static const int SEQUENCE_NAMES_OFFSET{ 2064 };
		static const int SONGS_OFFSET{ 3846 };
		static const int SEQUENCES_OFFSET{ 14406 };
		Header* header = nullptr;
		Defaults* defaults = nullptr;
		Sequencer* sequencer = nullptr;
		Count* count = nullptr;
		MidiInput* midiInput = nullptr;
		MidiSyncMisc* midiSyncMisc = nullptr;
		Misc* misc = nullptr;
		SequenceNames* seqNames = nullptr;
		std::vector<Sequence*> sequences;
		std::vector<Song*> songs;
		std::vector<char> saveBytes;

	public:
		std::vector<Sequence*> getAllSequences();
		Defaults* getDefaults();
		Sequencer* getSequencer();
		Count* getCount();
		MidiInput* getMidiInput();
		MidiSyncMisc* getMidiSync();
		Misc* getMisc();
		SequenceNames* getSeqNames();
		std::vector<Song*> getSongs();

	private:
		mpc::Mpc& mpc;
		std::vector<Sequence*> readSequences(std::vector<char> trimmedSeqsArray);

	public:
		std::vector<char> getBytes();

	public:
		AllParser(mpc::Mpc& mpc, mpc::disk::MpcFile* file);
		AllParser(mpc::Mpc& mpc, std::string allName);
		~AllParser();

	};
}
