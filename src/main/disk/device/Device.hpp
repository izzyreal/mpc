#pragma once

#include <thirdp/any.hpp>

#include <string>

namespace mpc {
	namespace disk {
		namespace device {

			class Device
			{
			public:
				virtual nonstd::any getRoot() = 0;
				virtual void close() = 0;
				virtual void flush() = 0;
				virtual nonstd::any getFileSystem() = 0;
				virtual std::string getAbsolutePath() = 0;
				virtual int getSize() = 0;
				virtual std::string getVolumeName() = 0;
				virtual ~Device() {};
			};

		}
	}
}
