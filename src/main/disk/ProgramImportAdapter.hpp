#pragma once

#include <memory>
#include <vector>

namespace mpc {

	namespace sampler {
		class Sampler;
		class Program;
		class NoteParameters;
	}

	namespace disk {

		class ProgramImportAdapter
		{

		private:
			std::weak_ptr<mpc::sampler::Sampler> sampler{};
			std::weak_ptr<mpc::sampler::Program> result{};
			std::vector<int> soundsDestIndex{};

		private:
			void processNoteParameters(mpc::sampler::NoteParameters* np);
			void initMixer(int note);

		public:
			std::weak_ptr<mpc::sampler::Program> get();

			ProgramImportAdapter(std::weak_ptr<mpc::sampler::Sampler> sampler,
				std::weak_ptr<mpc::sampler::Program> inputProgram,
				std::vector<int> soundsDestIndex,
				std::vector<int> unavailableSoundIndices
			);
		};

	}
}
