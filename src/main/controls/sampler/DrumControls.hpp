#pragma once
#include <controls/sampler/AbstractSamplerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sampler {

			class DrumControls
				: public AbstractSamplerControls
			{

			public:
				typedef AbstractSamplerControls super;
				void function(int f) override;
				void turnWheel(int i) override;

				DrumControls(mpc::Mpc* mpc);

			};

		}
	}
}
