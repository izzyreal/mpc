#pragma once
#include <controls/disk/AbstractDiskControls.hpp>

namespace mpc {
	namespace controls {
		namespace disk {
			namespace window {

				class SaveASoundControls
					: public mpc::controls::disk::AbstractDiskControls
				{

				public:
					typedef mpc::controls::disk::AbstractDiskControls super;
					void turnWheel(int i) override;
					void function(int i) override;

					SaveASoundControls();

				};

			}
		}
	}
}
