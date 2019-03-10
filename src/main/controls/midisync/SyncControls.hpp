#pragma once
#include <controls/midisync/AbstractMidiSyncControls.hpp>

namespace mpc {
	class Mpc;
	namespace controls {
		namespace midisync {

			class SyncControls
				: public AbstractMidiSyncControls
			{

			public:
				typedef AbstractMidiSyncControls super;
				void turnWheel(int i) override;
				void function(int i) override;

			public:
				SyncControls(mpc::Mpc* mpc);
				~SyncControls();
	
			};

		}
	}
}
