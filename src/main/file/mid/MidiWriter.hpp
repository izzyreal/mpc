#pragma once

#include <midi/event/NoteOn.hpp>
#include <midi/event/NoteOff.hpp>
#include <midi/MidiFile.hpp>
#include <midi/MidiTrack.hpp>

#include <memory>

namespace mpc::sequencer {
	class Sequence;
}

namespace mpc::file::mid {

	class MidiWriter
	{

	private:
		std::vector<std::shared_ptr<mpc::midi::event::NoteOn>> noteOffs{};
		std::vector<std::shared_ptr<mpc::midi::event::NoteOff>> variations{};
		std::vector<std::shared_ptr<mpc::midi::event::NoteOn>> noteOns{};
		std::vector<std::shared_ptr<mpc::midi::event::MidiEvent>> miscEvents{};
		mpc::sequencer::Sequence* sequence{};
		std::unique_ptr<mpc::midi::MidiFile> mf{};

	private:
		void addNoteOn(std::shared_ptr<mpc::midi::event::NoteOn> noteOn);
		void createDeltas(std::weak_ptr<mpc::midi::MidiTrack> mt);

	public:
		void writeToFile(string path);

	public:
		MidiWriter(mpc::sequencer::Sequence* sequence);
	};
}
