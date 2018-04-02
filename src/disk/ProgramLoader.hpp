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

		class ProgramLoader
		{

		private:
			std::thread loadProgramThread{};
			std::weak_ptr<mpc::sampler::Program> result{};
			mpc::Mpc* mpc{ nullptr };
			MpcFile* file{ nullptr };
			bool replace{ false };

		public:
			void loadProgram();

		private:
			static void static_loadProgram(void* this_p);
			
			void loadSound(std::string soundFileName, std::string ext, MpcFile* soundFile, std::vector<int>* soundsDestIndex, mpc::Mpc* mpc, bool replace, int loadSoundIndex);
			void showPopup(std::string name, std::string ext, int sampleSize);
			void notfound(std::string soundFileName, std::string ext);

		public:
			std::weak_ptr<mpc::sampler::Program> get();

			ProgramLoader(mpc::Mpc* mpc, MpcFile* file, bool replace);
			~ProgramLoader();
		};

	}
}
