#pragma once
#include <controls/vmpc/AbstractVmpcControls.hpp>

namespace mpc {
	namespace controls {
		namespace vmpc {

			class BufferSizeControls
				: public AbstractVmpcControls
			{

			public:
				typedef AbstractVmpcControls super;

			private:
				int reset{ -1 };

			public:
				void function(int i) override;
				void turnWheel(int i) override;

				BufferSizeControls(mpc::Mpc* mpc);

			};

		}
	}
}
