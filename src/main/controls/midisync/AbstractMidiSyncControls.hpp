#pragma once
#include <controls/BaseControls.hpp>

namespace mpc {
	
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
				: public mpc::controls::BaseControls
			{

			public:
				typedef mpc::controls::BaseControls super;

			protected:
				mpc::ui::midisync::MidiSyncGui* midiSyncGui{ nullptr };
				mpc::audiomidi::MpcMidiPorts* mpcMidiPorts{ nullptr };

			public:
				AbstractMidiSyncControls();
				virtual ~AbstractMidiSyncControls();

			};


		}
	}
}
