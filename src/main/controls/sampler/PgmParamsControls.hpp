#pragma once
#include <controls/sampler/AbstractSamplerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sampler {

			class PgmParamsControls
				: public AbstractSamplerControls
			{

			public:
				typedef AbstractSamplerControls super;
				void function(int i) override;
				void turnWheel(int i) override;
				void openWindow() override;

				PgmParamsControls();

			};

		}
	}
}
