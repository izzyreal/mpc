#pragma once

#include <vector>
#include <string>

namespace mpc::ui::misc {
	class PunchGui;
	class SecondSeqGui;
	class TransGui;
}

namespace mpc::ui::other {
	class OthersGui;
}

namespace mpc::ui::midisync {
	class MidiSyncGui;
}

namespace mpc::ui {

	class Uis
	{

	private:
		mpc::ui::midisync::MidiSyncGui* midiSyncGui = nullptr;
		mpc::ui::other::OthersGui* othersGui = nullptr;
		mpc::ui::misc::PunchGui* punchGui = nullptr;
		mpc::ui::misc::SecondSeqGui* secondSeqGui = nullptr;
		mpc::ui::misc::TransGui* transGui = nullptr;

	public:
		mpc::ui::midisync::MidiSyncGui* getMidiSyncGui();
		mpc::ui::other::OthersGui* getOthersGui();
		mpc::ui::misc::TransGui* getTransGui();
		mpc::ui::misc::SecondSeqGui* getSecondSeqGui();
		mpc::ui::misc::PunchGui* getPunchGui();

	private:
		std::vector<std::string> someNoteNames{ "C.", "C#", "D.", "D#", "E.", "F.", "F#", "G.", "G#", "A.", "A#", "B." };

	public:
		static std::vector<std::string> noteNames;

	public:
		Uis();
		~Uis();

	};
}
