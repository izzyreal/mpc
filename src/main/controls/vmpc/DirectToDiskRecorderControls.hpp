#pragma once
#include <controls/vmpc/AbstractVmpcControls.hpp>

namespace mpc {
	namespace controls {
		namespace vmpc {

			class DirectToDiskRecorderControls
				: public AbstractVmpcControls
			{

			public:
				typedef AbstractVmpcControls super;
				void turnWheel(int i) override;
				void function(int i) override;

				DirectToDiskRecorderControls();

			};

		}
	}
}
