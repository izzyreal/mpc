#pragma once

#include <disk/device/Device.hpp>
//#include <de/waldheinz/fs/FileSystem.hpp>
//#include <de/waldheinz/fs/fat/AkaiFatLfnDirectory.hpp>

namespace mpc {
	namespace disk {

		class RawDisk;

		namespace device {

			class RawWindowsDevice
				: public virtual Device
			{

			private:
				//static ::de::waldheinz::fs::util::RawWindowsVolume* rawVolume_;
				//static ::de::waldheinz::fs::FileSystem* fs_;
				std::string driveLetter{ "" };
				int totalSpace{ 0 };

			public:
				//::de::waldheinz::fs::FileSystem* getFileSystem() override;
				//::de::waldheinz::fs::fat::AkaiFatLfnDirectory* getRoot() override;
				std::any getFileSystem() override;
				std::any getRoot() override;
				void close() override;
				void flush() override;
				std::string getAbsolutePath() override;
				int getSize() override;
				std::string getVolumeName() override;

				RawWindowsDevice(std::string driveLetter, mpc::disk::RawDisk* disk, int totalSpace);

			private:
//				static ::de::waldheinz::fs::util::RawWindowsVolume*& rawVolume();
//				static ::de::waldheinz::fs::FileSystem*& fs();
			};

		}
	}
}
