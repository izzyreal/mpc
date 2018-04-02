#pragma once
#include <controls/sampler/AbstractSamplerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sampler {

			class PgmAssignControls
				: public AbstractSamplerControls
			{

			public:
				typedef AbstractSamplerControls super;
				void function(int i) override;
				void turnWheel(int i) override;
				void openWindow() override;

				PgmAssignControls(mpc::Mpc* mpc);

			};

		}
	}
}
