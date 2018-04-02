#pragma once

#include <thread>
#include <memory>
#include <vector>
#include <string>

namespace mpc {

	class Mpc;

	namespace sampler {
		class Program;
	}

	namespace disk {
		class MpcFile;
	}

	namespace file {
		namespace aps {

			class ApsLoader
			{

			private:
				mpc::Mpc* mpc;
				std::thread loadThread{};
				mpc::disk::MpcFile* file{ nullptr };
				
			private:
				void load();

			private:
				static void static_load(void* this_p);

				void loadSound(std::string soundFileName, std::string ext, mpc::disk::MpcFile* soundFile, mpc::Mpc* mpc, bool replace, int loadSoundIndex);
				void showPopup(std::string name, std::string ext, int sampleSize);

			public:
				ApsLoader(mpc::Mpc* mpc, mpc::disk::MpcFile* file);
				~ApsLoader();

			};

		}
	}
}
