#pragma once

#include <vector>
#include <string>

namespace mpc::ui::midisync {
	class MidiSyncGui;
}

namespace mpc::ui {

	class Uis
	{

	private:
		mpc::ui::midisync::MidiSyncGui* midiSyncGui = nullptr;

	public:
		mpc::ui::midisync::MidiSyncGui* getMidiSyncGui();

	private:
		std::vector<std::string> someNoteNames{ "C.", "C#", "D.", "D#", "E.", "F.", "F#", "G.", "G#", "A.", "A#", "B." };

	public:
		static std::vector<std::string> noteNames;

	public:
		Uis();
		~Uis();

	};
}
