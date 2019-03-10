#pragma once
#include <controls/sampler/AbstractSamplerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sampler {

			class InitPadAssignControls
				: public AbstractSamplerControls
			{

			public:
				typedef AbstractSamplerControls super;
				void turnWheel(int i) override;
				void function(int i) override;

				InitPadAssignControls(mpc::Mpc* mpc);

			};

		}
	}
}
