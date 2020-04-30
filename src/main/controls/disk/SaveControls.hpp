#pragma once
#include <controls/disk/AbstractDiskControls.hpp>

namespace mpc {
	namespace controls {
		namespace disk {

			class SaveControls
				: public AbstractDiskControls
			{

			public:
				typedef AbstractDiskControls super;
				void function(int i) override;
				void turnWheel(int i) override;

				SaveControls();

			};

		}
	}
}
