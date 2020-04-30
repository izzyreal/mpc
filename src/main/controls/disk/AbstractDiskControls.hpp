#pragma once
#include <controls/BaseControls.hpp>

#include <memory>

namespace mpc {
	
	namespace disk {
		class AbstractDisk;
		class MpcFile;
	}

	namespace ui {

		namespace sampler {
			class SoundGui;
		}
		
		namespace vmpc {
			class DeviceGui;
		}

		class NameGui;

		namespace disk {
			class DiskGui;

			namespace window {
				class DirectoryGui;
				class DiskWindowGui;

			}
		}
	}

	namespace controls {
		namespace disk {

			class AbstractDiskControls
				: public mpc::controls::BaseControls
			{

			public:
				typedef mpc::controls::BaseControls super;

			protected:
				mpc::ui::disk::window::DirectoryGui* directoryGui{ nullptr };
				mpc::ui::disk::DiskGui* diskGui{ nullptr };
				mpc::ui::disk::window::DiskWindowGui* diskWindowGui{ nullptr };
				mpc::ui::NameGui* nameGui{ nullptr };
				std::weak_ptr<mpc::disk::AbstractDisk> disk{};
				mpc::ui::vmpc::DeviceGui* deviceGui{ nullptr };
				mpc::ui::sampler::SoundGui* soundGui{ nullptr };
				mpc::disk::MpcFile* selectedFile{ nullptr };

			protected:
				void init() override;

			public:
				AbstractDiskControls();
				virtual ~AbstractDiskControls();

			};

		}
	}
}
