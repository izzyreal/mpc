#pragma once
#include <controls/disk/AbstractDiskControls.hpp>

namespace mpc {
	namespace controls {
		namespace disk {

			class FormatControls
				: public AbstractDiskControls
			{

			public:
				typedef AbstractDiskControls super;
				void function(int i) override;

				FormatControls();
			};
	
		}
	}
}
