#pragma once

#include <vector>
#include <string>

namespace mpc::ui {
	class NameGui;
}

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

namespace mpc::ui::disk {
	class DiskGui;
}

namespace mpc::ui::disk::window {
	class DiskWindowGui;
	class DirectoryGui;
}

namespace mpc::ui::sampler {
	class SamplerGui;
}

namespace mpc::ui::sampler::window {
	class SamplerWindowGui;
	class EditSoundGui;
}

namespace mpc::ui::midisync {
	class MidiSyncGui;
}

namespace mpc::ui {

	class Uis
	{

	private:
		mpc::ui::NameGui* nameGui = nullptr;
		mpc::ui::sampler::SamplerGui* samplerGui = nullptr;

		mpc::ui::sampler::window::EditSoundGui* editSoundGui = nullptr;
		mpc::ui::sampler::window::SamplerWindowGui* samplerWindowGui = nullptr;

		mpc::ui::disk::DiskGui* diskGui = nullptr;
		mpc::ui::disk::window::DirectoryGui* directoryGui = nullptr;
		mpc::ui::disk::window::DiskWindowGui* diskWindowGui = nullptr;

		mpc::ui::midisync::MidiSyncGui* midiSyncGui = nullptr;

		mpc::ui::other::OthersGui* othersGui = nullptr;

		mpc::ui::misc::PunchGui* punchGui = nullptr;
		mpc::ui::misc::SecondSeqGui* secondSeqGui = nullptr;
		mpc::ui::misc::TransGui* transGui = nullptr;

		mpc::ui::vmpc::DeviceGui* deviceGui = nullptr;
		mpc::ui::vmpc::DirectToDiskRecorderGui* d2dRecorderGui = nullptr;

	public:
		mpc::ui::midisync::MidiSyncGui* getMidiSyncGui();
		mpc::ui::sampler::window::EditSoundGui* getEditSoundGui();
		mpc::ui::disk::DiskGui* getDiskGui();
		mpc::ui::disk::window::DirectoryGui* getDirectoryGui();
		mpc::ui::disk::window::DiskWindowGui* getDiskWindowGui();
		mpc::ui::sampler::window::SamplerWindowGui* getSamplerWindowGui();
		mpc::ui::sampler::SamplerGui* getSamplerGui();
		mpc::ui::NameGui* getNameGui();

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
