#pragma once

#include <vector>
#include <string>

namespace mpc::ui::vmpc {
	class DirectToDiskRecorderGui;
	class DeviceGui;
}

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

		mpc::ui::vmpc::DeviceGui* deviceGui = nullptr;
		mpc::ui::vmpc::DirectToDiskRecorderGui* d2dRecorderGui = nullptr;

	public:
		mpc::ui::midisync::MidiSyncGui* getMidiSyncGui();

		mpc::ui::other::OthersGui* getOthersGui();

		mpc::ui::vmpc::DeviceGui* getDeviceGui();
		mpc::ui::vmpc::DirectToDiskRecorderGui* getD2DRecorderGui();

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
