#pragma once
#include <controls/disk/AbstractDiskControls.hpp>

#include <file/aps/ApsLoader.hpp>

#include <memory>

namespace mpc {

	namespace controls {
		namespace disk {
			namespace window {

				class LoadApsFileControls
					: public mpc::controls::disk::AbstractDiskControls
				{

				private:
					std::unique_ptr<mpc::file::aps::ApsLoader> apsLoader;

				public:
					typedef mpc::controls::disk::AbstractDiskControls super;
					void function(int i) override;

					LoadApsFileControls(mpc::Mpc* mpc);

				};

			}
		}
	}
}
