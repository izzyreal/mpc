#pragma once
#include <controls/sampler/AbstractSamplerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sampler {
			namespace window {

				class VeloEnvFilterControls
					: public mpc::controls::sampler::AbstractSamplerControls
				{

				public:
					typedef mpc::controls::sampler::AbstractSamplerControls super;
					void turnWheel(int i) override;

					VeloEnvFilterControls();

				};

			}
		}
	}
}
