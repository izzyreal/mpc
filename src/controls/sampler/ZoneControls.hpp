#pragma once
#include <controls/sampler/AbstractSamplerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sampler {

			class ZoneControls
				: public AbstractSamplerControls
			{

			public:
				typedef AbstractSamplerControls super;

			public:
				void init() override;

			public:
				void openWindow() override;
				void function(int f) override;
				void turnWheel(int i) override;

				ZoneControls(mpc::Mpc* mpc);

			};

		}
	}
}
