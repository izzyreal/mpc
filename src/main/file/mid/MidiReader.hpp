#pragma once

#include <midi/MidiFile.hpp>

#include <memory>
#include <string>
#include <vector>

namespace mpc {

	

	namespace midi {

		namespace event {
			class NoteOn;
		}
	}

	namespace sequencer {
		class Sequence;
		class NoteEvent;
	}

	namespace disk {
		class MpcFile;
	}

	namespace file {
		namespace mid {
			
			class MidiReader
			{

			private:
				std::unique_ptr<mpc::midi::MidiFile> midiFile{};
				std::weak_ptr<mpc::sequencer::Sequence> dest{};
				

			public:
				void parseSequence();

			private:
				static bool isInteger(std::string s);
				int getNumberOfNoteOns(int noteValue, std::vector<std::shared_ptr<mpc::midi::event::NoteOn>> allNotes);
				int getNumberOfNotes(int noteValue, std::vector<std::shared_ptr<mpc::sequencer::NoteEvent>> allNotes);

			public:
				MidiReader(mpc::disk::MpcFile* file, std::weak_ptr<mpc::sequencer::Sequence> dest);
			};

		}
	}
}
