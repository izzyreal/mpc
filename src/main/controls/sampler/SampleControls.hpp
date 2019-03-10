#pragma once
#include <controls/sampler/AbstractSamplerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sampler {

			class SampleControls
				: public AbstractSamplerControls
			{

			public:
				typedef AbstractSamplerControls super;
				void function(int i) override;
				void turnWheel(int i) override;
				void left() override;
				void right() override;
				void up() override;
				void down() override;

			public:
				SampleControls(mpc::Mpc* mpc);

			};

		}
	}
}
