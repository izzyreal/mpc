#pragma once
#include <controls/vmpc/AbstractVmpcControls.hpp>

namespace mpc {
	namespace controls {
		namespace vmpc {

			class RecordingFinishedControls
				: public AbstractVmpcControls
			{

			public:
				typedef AbstractVmpcControls super;
				void function(int i) override;

				RecordingFinishedControls(mpc::Mpc* mpc);

			};

		}
	}
}
