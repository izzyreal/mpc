#pragma once
#include <disk/device/Device.hpp>

#include <memory>

namespace moduru {
	namespace file {
		class Directory;
	}
}

namespace mpc {
	namespace disk {
		namespace device {

			class StdDevice
				: public virtual Device
			{

			private:
				bool valid{ false };
				std::shared_ptr<moduru::file::Directory> root{};

			public:
				boost::any getRoot() override;
				virtual bool isValid();
				void close() override;
				void flush() override;
				boost::any getFileSystem() override;
				std::string getAbsolutePath() override;
				int getSize() override;
				std::string getVolumeName() override;

				StdDevice(std::string rootPath);
				~StdDevice();

			};

		}
	}
}
