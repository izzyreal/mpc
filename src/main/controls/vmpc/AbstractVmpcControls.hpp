#pragma once
#include <controls/AbstractControls.hpp>

namespace mpc::ui::midisync {
	class MidiSyncGui;
}

namespace mpc::ui::vmpc {
	class DirectToDiskRecorderGui;
}
namespace mpc::audiomidi {
	class MpcMidiPorts;
	class AudioMidiServices;
}

namespace mpc::controls::vmpc {

	class AbstractVmpcControls
		: public mpc::controls::AbstractControls
	{

	public:
		typedef mpc::controls::AbstractControls super;

	protected:
		std::weak_ptr<mpc::audiomidi::AudioMidiServices> ams;
		mpc::audiomidi::MpcMidiPorts* mpcMidiPorts = nullptr;
		mpc::ui::midisync::MidiSyncGui* midiSyncGui = nullptr;
		mpc::ui::vmpc::DirectToDiskRecorderGui* d2dRecorderGui = nullptr;

	protected:
		void init() override;

	protected:
		void checkAllTimes(int notch);

	public:
		AbstractVmpcControls(mpc::Mpc* mpc);
		virtual ~AbstractVmpcControls();

	};
}
