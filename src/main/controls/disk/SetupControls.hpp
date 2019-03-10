#pragma once
#include <controls/disk/AbstractDiskControls.hpp>

namespace mpc {

	namespace controls {
		namespace disk {

			class SetupControls
				: public AbstractDiskControls
			{
			public:
				typedef AbstractDiskControls super;
				SetupControls(mpc::Mpc* mpc);

			};

		}
	}
}
