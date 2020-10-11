#pragma once

#include <disk/SoundSaver.hpp>

#include <memory>
#include <string>

namespace mpc { class Mpc; }

namespace mpc::disk {

	class ApsSaver
	{

	private:
		mpc::Mpc& mpc;
		std::string apsFileName = "";
		std::unique_ptr<mpc::disk::SoundSaver> soundSaver;

	private:
		void saveAps();

	public:
		ApsSaver(mpc::Mpc& mpc, std::string apsFileName);

	};
}
