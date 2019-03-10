#pragma once
#include <controls/misc/AbstractMiscControls.hpp>

namespace mpc {
	class Mpc;
	namespace controls {
		namespace misc {

			class PunchControls
				: public AbstractMiscControls
			{

			public:
				typedef AbstractMiscControls super;
				void turnWheel(int i) override;
				void function(int i) override;

			public:
				PunchControls(mpc::Mpc* mpc);
				~PunchControls();

			};

		}
	}
}
