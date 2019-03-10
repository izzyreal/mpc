#pragma once

#include <memory>
#include <string>

#include <disk/SoundSaver.hpp>

namespace mpc {

	class Mpc;

	namespace file {
		namespace aps {

			class ApsSaver
			{

			private:
				mpc::Mpc* mpc{ nullptr };
				std::string apsFileName{""};
				std::unique_ptr<mpc::disk::SoundSaver> soundSaver;

			private:
				void saveAps();

			public:
				ApsSaver(mpc::Mpc* mpc, std::string apsFileName);

			};

		}
	}
}
