#pragma once

#include <midi/MidiFile.hpp>

#include <memory>
#include <string>
#include <vector>

namespace mpc {
	class Mpc;
}

namespace mpc::midi::event {
	class NoteOn;
}

namespace mpc::sequencer {
	class Sequence;
	class NoteEvent;
}

namespace mpc::disk {
	class MpcFile;
}

namespace mpc::file::mid {
	class MidiReader
	{

	private:
		std::unique_ptr<mpc::midi::MidiFile> midiFile;
		std::weak_ptr<mpc::sequencer::Sequence> dest;


	public:
		void parseSequence(mpc::Mpc& mpc);

	private:
		static bool isInteger(std::string s);
		int getNumberOfNoteOns(int noteValue, std::vector<std::shared_ptr<mpc::midi::event::NoteOn>> allNotes);
		int getNumberOfNotes(int noteValue, std::vector<std::shared_ptr<mpc::sequencer::NoteEvent>> allNotes);

	public:
		MidiReader(mpc::disk::MpcFile* file, std::weak_ptr<mpc::sequencer::Sequence> dest);
	};
}
