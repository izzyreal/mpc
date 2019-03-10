#pragma once
#include <controls/disk/AbstractDiskControls.hpp>

#include <thread>

namespace mpc {
	namespace controls {
		namespace disk {
			namespace dialog {

				class DeleteFolderControls
					: public mpc::controls::disk::AbstractDiskControls
				{
				private:
					std::thread deleteFolderThread{};
					static void static_deleteFolder(void* this_p);
					void deleteFolder();

				public:
					typedef mpc::controls::disk::AbstractDiskControls super;
					void function(int i) override;

				public:
					DeleteFolderControls(mpc::Mpc* mpc);
					~DeleteFolderControls();

				};

			}
		}
	}
}
