#pragma once
#include <controls/disk/AbstractDiskControls.hpp>

namespace mpc {
	namespace controls {
		namespace disk {

			class LoadControls
				: public AbstractDiskControls
			{

			public:
				typedef AbstractDiskControls super;
				void function(int i) override;
				void openWindow() override;
				void turnWheel(int i) override;

				LoadControls(mpc::Mpc* mpc);

			};

		}
	}
}
