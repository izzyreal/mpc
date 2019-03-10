#pragma once
#include <controls/disk/AbstractDiskControls.hpp>

namespace mpc {
	namespace controls {
		namespace disk {
			namespace window {

				class MPC2000XLAllFileControls
					: public mpc::controls::disk::AbstractDiskControls
				{

				public:
					typedef mpc::controls::disk::AbstractDiskControls super;
					void function(int i) override;

					MPC2000XLAllFileControls(mpc::Mpc* mpc);

				};

			}
		}
	}
}
