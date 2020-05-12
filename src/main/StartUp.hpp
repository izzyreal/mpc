#pragma once

#include <string>

namespace mpc {
	
	class StartUp
	{

	public:
		static std::string home;
		static std::string resPath;
		static std::string tempPath;
		static std::string storesPath;
		static std::string logFilePath;

	public:
		static std::string altResPath();

	};
}
