#pragma once

#include <memory>
#include <vector>

namespace moduru::file
{
	class File;
}

namespace mpc
{
	class Mpc;
}

namespace mpc::file::wav
{
	class WavFile;
}

namespace mpc::sampler
{
	class Sound;
}

namespace mpc::disk
{
	class MpcFile;
}

namespace mpc::disk
{
	class SoundLoader
	{
	private:
		static float rateToTuneBase;
		mpc::Mpc& mpc;
        bool partOfProgram = false;
		std::vector<std::weak_ptr<mpc::sampler::Sound>> sounds;
		bool preview = false;
		bool replace = false;
        int size = 0;

	public:
		static void getSampleDataFromWav(std::weak_ptr<moduru::file::File> wavFile, std::vector<float>* dest);

		void setPartOfProgram(bool);
		int loadSound(std::weak_ptr<MpcFile>);
		void setPreview(bool);
		double logOfBase(float num, float base);
		int getSize();

		SoundLoader(mpc::Mpc& mpc);
		SoundLoader(mpc::Mpc& mpc, std::vector<std::weak_ptr<mpc::sampler::Sound>> sounds, bool replace);

	};
}
