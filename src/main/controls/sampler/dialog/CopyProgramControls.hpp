#pragma once
#include <controls/sampler/AbstractSamplerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sampler {
			namespace dialog {

				class CopyProgramControls
					: public mpc::controls::sampler::AbstractSamplerControls
				{

				public:
					typedef mpc::controls::sampler::AbstractSamplerControls super;
					void function(int i) override;
					void turnWheel(int i) override;

					CopyProgramControls();

				};

			}
		}
	}
}
