#pragma once

//#include <de/waldheinz/fs/fwd-Mpc.hpp>
//#include <de/waldheinz/fs/fat/fwd-Mpc.hpp>
//#include <de/waldheinz/fs/util/fwd-Mpc.hpp>
//#include <de/waldheinz/fs/FileSystem.hpp>
//#include <de/waldheinz/fs/fat/AkaiFatLfnDirectory.hpp>
#include <disk/device/Device.hpp>

namespace mpc {
	namespace disk {
		namespace device {

			class RawOsxDevice
				: public virtual Device
			{

			private:
				std::string absolutePath{};
				//static ::de::waldheinz::fs::util::RawOsxVolume* rawVolume_;
				//static ::de::waldheinz::fs::FileSystem* fs_;

			public:
				std::any getRoot() override;
				std::any getFileSystem() override;
				void close() override;
				void flush() override;
				std::string getAbsolutePath() override;
				int getSize() override;
				std::string getVolumeName() override;

				RawOsxDevice(std::string fileName);

			private:
				//static ::de::waldheinz::fs::util::RawOsxVolume*& rawVolume();
				//static ::de::waldheinz::fs::FileSystem*& fs();
			};

		}
	}
}
