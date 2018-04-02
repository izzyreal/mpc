#pragma once
#include <controls/sampler/AbstractSamplerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sampler {

			class SndParamsControls
				: public AbstractSamplerControls
			{

			public:
				typedef AbstractSamplerControls super;
				void openWindow() override;
				void function(int f) override;
				void turnWheel(int i) override;

				SndParamsControls(mpc::Mpc* mpc);

			};

		}
	}
}
