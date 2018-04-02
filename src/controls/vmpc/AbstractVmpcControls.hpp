#pragma once
#include <controls/AbstractControls.hpp>

namespace mpc {

	namespace ui {
		namespace midisync {
			class MidiSyncGui;
		}
		namespace vmpc {
			class MidiGui;
			class AudioGui;
			class DirectToDiskRecorderGui;
		}
	}

	namespace audiomidi {
		class MpcMidiPorts;
		class AudioMidiServices;
	}

	namespace controls {
		namespace vmpc {

			class AbstractVmpcControls
				: public mpc::controls::AbstractControls
			{

			public:
				typedef mpc::controls::AbstractControls super;

			protected:
				std::weak_ptr<mpc::audiomidi::AudioMidiServices> ams{};
				mpc::audiomidi::MpcMidiPorts* mpcMidiPorts{ nullptr };
				mpc::ui::midisync::MidiSyncGui* midiSyncGui{ nullptr };
				mpc::ui::vmpc::MidiGui* midiGui{ nullptr };
				mpc::ui::vmpc::AudioGui* audioGui{ nullptr };
				mpc::ui::vmpc::DirectToDiskRecorderGui* d2dRecorderGui{ nullptr };

			protected:
				void init() override;

			protected:
				void checkAllTimes(int notch);

			public:
				AbstractVmpcControls(mpc::Mpc* mpc);
				virtual ~AbstractVmpcControls();

			};

		}
	}
}
