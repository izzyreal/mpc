#pragma once
#include <controls/sampler/AbstractSamplerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sampler {

			class SelectDrumControls
				: public AbstractSamplerControls
			{

			public:
				typedef AbstractSamplerControls super;
				void function(int i) override;

				SelectDrumControls();

			};

		}
	}
}
