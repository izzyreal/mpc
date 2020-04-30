#pragma once
#include <controls/disk/AbstractDiskControls.hpp>

namespace mpc {
	namespace controls {
		namespace disk {
			namespace dialog {

				class DeleteAllFilesControls
					: public mpc::controls::disk::AbstractDiskControls
				{

				public:
					void turnWheel(int i) override;
					void function(int i) override;

					DeleteAllFilesControls();
				};

			}
		}
	}
}
