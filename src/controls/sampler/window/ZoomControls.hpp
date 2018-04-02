#pragma once
#include <controls/sampler/AbstractSamplerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sampler {
			namespace window {

				class ZoomControls
					: public mpc::controls::sampler::AbstractSamplerControls
				{

				public:
					typedef mpc::controls::sampler::AbstractSamplerControls super;

				private:
					int sampleLength{ 0 };

				public:
					void init() override;
					void function(int i) override;
					void turnWheel(int i) override;

				public:
					ZoomControls(mpc::Mpc* mpc);

				};

			}
		}
	}
}
