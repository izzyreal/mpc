#pragma once
#include <controls/disk/AbstractDiskControls.hpp>
#include <file/all/AllParser.hpp>

#include <memory>

namespace mpc {

	namespace controls {
		namespace disk {
			namespace window {

				class SaveAllFileControls
					: public mpc::controls::disk::AbstractDiskControls
				{
				private:
					std::unique_ptr<mpc::file::all::AllParser> allParser{};

				public:
					typedef mpc::controls::disk::AbstractDiskControls super;
					void turnWheel(int i) override;
					void function(int i) override;

					SaveAllFileControls();

				};

			}
		}
	}
}
