#pragma once
#include <controls/vmpc/AbstractVmpcControls.hpp>

namespace mpc {
	namespace controls {
		namespace vmpc {

			class AudioControls
				: public AbstractVmpcControls
			{

			public:
				typedef AbstractVmpcControls super;
				void openWindow() override;
				void turnWheel(int i) override;
				void function(int i) override;

			public:
				AudioControls(mpc::Mpc* mpc);
				~AudioControls();

			};

		}
	}
}
