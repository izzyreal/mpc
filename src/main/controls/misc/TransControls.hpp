#pragma once

#include <controls/misc/AbstractMiscControls.hpp>

namespace mpc {
	namespace controls {
		namespace misc {

			class TransControls
				: public AbstractMiscControls
			{

			public:
				typedef AbstractMiscControls super;
				void function(int i) override;
				void turnWheel(int i) override;

			public:
				TransControls();
				~TransControls();

			};

		}
	}
}
