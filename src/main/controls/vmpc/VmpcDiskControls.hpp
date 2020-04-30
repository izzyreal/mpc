#pragma once
#include <controls/disk/AbstractDiskControls.hpp>

namespace mpc {
	namespace controls {
		namespace vmpc {

			class VmpcDiskControls
				: public mpc::controls::disk::AbstractDiskControls
			{

			public:
				void function(int i) override;
				void turnWheel(int i) override;

				VmpcDiskControls();

			};

		}
	}
}
