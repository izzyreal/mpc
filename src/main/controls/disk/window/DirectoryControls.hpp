#pragma once

#include <controls/disk/AbstractDiskControls.hpp>

namespace mpc {
	namespace controls {
		namespace disk {
			namespace window {

				class DirectoryControls
					: public mpc::controls::disk::AbstractDiskControls
				{

				public:
					typedef mpc::controls::disk::AbstractDiskControls super;
					void function(int f) override;
					void left() override;
					void right() override;
					void up() override;
					void down() override;
					void turnWheel(int i) override;

					DirectoryControls();

				};

			}
		}
	}
}
