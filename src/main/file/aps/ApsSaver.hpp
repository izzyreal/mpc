#pragma once

#include <memory>
#include <string>

#include <disk/SoundSaver.hpp>

namespace mpc {

	namespace file {
		namespace aps {

			class ApsSaver
			{

			private:
				
				std::string apsFileName{""};
				std::unique_ptr<mpc::disk::SoundSaver> soundSaver;

			private:
				void saveAps();

			public:
				ApsSaver(std::string apsFileName);

			};

		}
	}
}
