#pragma once

#include "mpc_types.hpp"

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
	class AllSequencer;
	class Count;
	class MidiInput;
	class MidiSyncMisc;
	class Misc;
	class SequenceNames;
	class AllSequence;
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
		AllSequencer* sequencer = nullptr;
		Count* count = nullptr;
		MidiInput* midiInput = nullptr;
		MidiSyncMisc* midiSyncMisc = nullptr;
		Misc* misc = nullptr;
        SequenceNames* seqNames = nullptr;
        std::vector<AllSequence*> sequences;
        std::vector<Song*> songs = std::vector<Song*>(20);
		std::vector<char> saveBytes;

	public:
		std::vector<AllSequence*> getAllSequences();
		Defaults* getDefaults();
		AllSequencer* getSequencer();
		Count* getCount();
		MidiInput* getMidiInput();
		MidiSyncMisc* getMidiSync();
		Misc* getMisc();
		SequenceNames* getSeqNames();
		std::vector<Song*> getSongs();

	private:
		mpc::Mpc& mpc;
        
        // Dangerous method, as the caller relies on the argument being a copy
		std::vector<AllSequence*> readSequences(std::vector<char> trimmedSeqsArray);

	public:
		std::vector<char>& getBytes();

	public:
		AllParser(mpc::Mpc&, const std::vector<char>& loadBytes);
		AllParser(mpc::Mpc&);
		~AllParser();

	};
}
