#pragma once
#include <controls/sampler/AbstractSamplerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sampler {
			namespace window {

				class SoundControls
					: public mpc::controls::sampler::AbstractSamplerControls
				{

				public:
					void turnWheel(int i) override;
					void function(int i) override;

					SoundControls();

				};

			}
		}
	}
}
