#pragma once

#include <memory>
#include <vector>

using namespace std;

namespace moduru::file {
	class File;
}

namespace mpc {
	class Mpc;
}

namespace mpc::file::wav {
	class WavFile;
}

namespace mpc::sampler {
	class Sound;
}

namespace mpc::disk {
	class MpcFile;
}

namespace mpc::disk {
	class SoundLoader
	{
	private:
		mpc::Mpc* mpc;
		static float rateToTuneBase;

		bool partOfProgram{ false };
		MpcFile* soundFile{ nullptr };
		vector<weak_ptr<mpc::sampler::Sound>> sounds{};
		bool preview{ false };
		bool replace{ false };
		int size{ 0 };

	public:
		static void getSampleDataFromWav(weak_ptr<moduru::file::File> wavFile, vector<float>* dest);

		void setPartOfProgram(bool b);
		int loadSound(MpcFile* f);
		void setPreview(bool b);
		double logOfBase(float num, float base);
		int getSize();

		SoundLoader(mpc::Mpc* mpc);
		SoundLoader(mpc::Mpc* mpc, vector<weak_ptr<mpc::sampler::Sound>> sounds, bool replace);

	};
}
