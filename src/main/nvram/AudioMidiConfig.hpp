#pragma once

#include <string>
#include <vector>

namespace mpc::nvram {

	class AudioMidiConfig {

	public:
		static void load(std::string& serverName, int& bufferSize);
		static void loadIO(std::vector<std::string>* ioNames);
		static void save(const std::vector<std::string>& ioNames, const int& bufferSize);

	};
}
