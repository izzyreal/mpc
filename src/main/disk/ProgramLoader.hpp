#pragma once

#include <thread>
#include <memory>
#include <vector>
#include <string>

namespace mpc::sampler
{
	class Program;
}

namespace mpc::disk
{
	class MpcFile;
}

namespace mpc::disk
{
	class ProgramLoader
	{
	private:
		std::thread loadProgramThread;
		std::weak_ptr<mpc::sampler::Program> result;

		MpcFile* file = nullptr;
		bool replace = false;

	public:
		void loadProgram();

	private:
		static void static_loadProgram(void* this_p);

		void loadSound(std::string soundFileName, std::string ext, MpcFile* soundFile, std::vector<int>* soundsDestIndex, bool replace, int loadSoundIndex);
		void showPopup(std::string name, std::string ext, int sampleSize);
		void notfound(std::string soundFileName, std::string ext);

	public:
		std::weak_ptr<mpc::sampler::Program> get();

		ProgramLoader(MpcFile* file, bool replace);
		~ProgramLoader();
	};
}
