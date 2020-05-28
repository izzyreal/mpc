#pragma once

#include <disk/SoundSaver.hpp>

#include <memory>
#include <string>

namespace mpc::disk {

	class ApsSaver
	{

	private:
		std::string apsFileName = "";
		std::unique_ptr<mpc::disk::SoundSaver> soundSaver;

	private:
		void saveAps();

	public:
		ApsSaver(std::string apsFileName);

	};
}
