#pragma once
#include <controls/sampler/AbstractSamplerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sampler {
			namespace window {

				class KeepOrRetryControls
					: public mpc::controls::sampler::AbstractSamplerControls
				{

				public:
					void function(int i) override;
					void turnWheel(int i) override;
					void mainScreen() override;

				public:
					KeepOrRetryControls(mpc::Mpc* mpc);

				};

			}
		}
	}
}
