#pragma once
#include <controls/disk/AbstractDiskControls.hpp>

#include <thread>

namespace mpc {
	namespace controls {
		namespace disk {
			namespace dialog {

				class DeleteFileControls
					: public mpc::controls::disk::AbstractDiskControls
				{

				private:
					static void static_delete(void * args);

					std::thread deleteThread;
					void deleteFile();

				public:
					typedef mpc::controls::disk::AbstractDiskControls super;
					void function(int i) override;

					DeleteFileControls();

				private:
					friend class DeleteFileThread;

				};

			}
		}
	}
}
