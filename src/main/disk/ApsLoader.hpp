#pragma once

#include <thread>
#include <memory>
#include <vector>
#include <string>

namespace mpc {
	
}

namespace mpc::sampler {
	class Program;
}

namespace mpc::disk {
	class MpcFile;
}

namespace mpc::disk {

	class ApsLoader
	{

	private:
		
		std::thread loadThread{};
		mpc::disk::MpcFile* file{ nullptr };

	private:
		void load();

	private:
		static void static_load(void* this_p);

		void loadSound(std::string soundFileName, std::string ext, mpc::disk::MpcFile* soundFile, bool replace, int loadSoundIndex);
		void showPopup(std::string name, std::string ext, int sampleSize);

	public:
		ApsLoader(mpc::disk::MpcFile* file);
		~ApsLoader();

	};
}
