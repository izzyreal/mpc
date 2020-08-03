#pragma once

#include <thread>
#include <memory>
#include <vector>
#include <string>

namespace mpc { class Mpc; }

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
		mpc::Mpc& mpc;
		std::thread loadProgramThread;
		std::weak_ptr<mpc::sampler::Program> result;

		MpcFile* file = nullptr;
		bool replace = false;

	public:
		void loadProgram();

	private:
		static void static_loadProgram(void* this_p);

		void loadSound(const std::string& soundFileName, const std::string& soundName, const std::string& ext, MpcFile* soundFile, std::vector<int>* soundsDestIndex, const bool replace, const int loadSoundIndex);
		void showPopup(std::string name, std::string ext, int sampleSize);
		void notfound(std::string soundFileName, std::string ext);

	public:
		std::weak_ptr<mpc::sampler::Program> get();

		ProgramLoader(mpc::Mpc& mpc, MpcFile* file, bool replace);
		~ProgramLoader();
	};
}
