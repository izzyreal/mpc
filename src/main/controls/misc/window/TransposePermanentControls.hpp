#pragma once

#include <controls/misc/AbstractMiscControls.hpp>

namespace mpc {
	class Mpc;
	namespace controls {
		namespace misc {
			namespace window {

				class TransposePermanentControls
					: public AbstractMiscControls
				{

				public:
					typedef AbstractMiscControls super;
					void function(int i) override;
					void turnWheel(int i) override;

				public:
					TransposePermanentControls(mpc::Mpc* mpc);
					~TransposePermanentControls();

				};

			}
		}
	}
}
