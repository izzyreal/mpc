#pragma once
#include <controls/AbstractControls.hpp>

namespace mpc {
	class Mpc;
	namespace audiomidi {
		class MpcMidiPorts;
	}

	namespace ui {
		namespace midisync {
			class MidiSyncGui;
		}
	}

	namespace controls {
		namespace midisync {

			class AbstractMidiSyncControls
				: public mpc::controls::AbstractControls
			{

			public:
				typedef mpc::controls::AbstractControls super;

			protected:
				mpc::ui::midisync::MidiSyncGui* midiSyncGui{ nullptr };
				mpc::audiomidi::MpcMidiPorts* mpcMidiPorts{ nullptr };

			public:
				AbstractMidiSyncControls(mpc::Mpc* mpc);
				virtual ~AbstractMidiSyncControls();

			};


		}
	}
}
