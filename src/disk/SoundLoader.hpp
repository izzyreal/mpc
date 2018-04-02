#pragma once

#include <memory>
#include <vector>

namespace moduru {
	namespace file {
		class File;
	}
}

namespace mpc {
	class Mpc;
	namespace file {
		namespace wav {
			class WavFile;
		}
	}

	namespace sampler {
		class Sound;
	}

	namespace disk {

		class MpcFile;

		class SoundLoader
		{
		private:
			mpc::Mpc* mpc;
			static float rateToTuneBase;

			bool partOfProgram{ false };
			MpcFile* soundFile{ nullptr };
			std::vector<std::weak_ptr<mpc::sampler::Sound>> sounds{};
			bool preview{ false };
			bool replace{ false };
			int size{ 0 };

		public:
			static std::vector<float> getSampleDataFromWav(std::weak_ptr<moduru::file::File> wavFile);
			static void getSampleDataFromWav(std::weak_ptr<moduru::file::File> wavFile, std::vector<float>* dest);

			void setPartOfProgram(bool b);
			int loadSound(MpcFile* f);
			void setPreview(bool b);
			double logOfBase(float num, float base);
			int getSize();

			SoundLoader(mpc::Mpc* mpc);
			SoundLoader(mpc::Mpc* mpc, std::vector<std::weak_ptr<mpc::sampler::Sound>> sounds, bool replace);

		};

	}
}
