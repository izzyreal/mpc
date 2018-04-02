#pragma once
#include <controls/vmpc/AbstractVmpcControls.hpp>

struct default_init_tag;

namespace mpc {
	namespace controls {
		namespace vmpc {

			class AudioMidiDisabledControls
				: public AbstractVmpcControls
			{

			public:
				typedef AbstractVmpcControls super;
				void function(int i) override;

				AudioMidiDisabledControls(mpc::Mpc* mpc);

			};

		}
	}
}
