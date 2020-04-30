#pragma once
#include <controls/sampler/AbstractSamplerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sampler {
			namespace window {

				class NumberOfZonesControls
					: public mpc::controls::sampler::AbstractSamplerControls
				{

				public:
					typedef mpc::controls::sampler::AbstractSamplerControls super;
					void function(int f) override;
					void turnWheel(int i) override;

					NumberOfZonesControls();

				};

			}
		}
	}
}
