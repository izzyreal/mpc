#pragma once
#include <controls/disk/AbstractDiskControls.hpp>

#include <memory>
#include <file/aps/ApsSaver.hpp>

namespace mpc {

	namespace controls {
		namespace disk {
			namespace window {

				class SaveApsFileControls
					: public mpc::controls::disk::AbstractDiskControls
				{

				private:
					std::unique_ptr<mpc::file::aps::ApsSaver> apsSaver{};

				public:
					typedef mpc::controls::disk::AbstractDiskControls super;
					void turnWheel(int i) override;
					void function(int i) override;

					SaveApsFileControls(mpc::Mpc* mpc);

				};

			}
		}
	}
}
