#pragma once
#include <controls/vmpc/AbstractVmpcControls.hpp>

namespace mpc {
	namespace controls {
		namespace vmpc {

			class RecordJamControls
				: public AbstractVmpcControls
			{

			public:
				typedef AbstractVmpcControls super;
				void function(int i) override;

				RecordJamControls();

			private:
//				friend class RecordJamControls_function_1;
			};

		}
	}
}
