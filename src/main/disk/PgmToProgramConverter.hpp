#pragma once

#include <memory>
#include <vector>
#include <string>

namespace mpc {

	namespace sampler {
		class Sampler;
		class Program;
	}

	namespace file {
		namespace pgmreader {
			class ProgramFileReader;
		}
	}

	namespace disk {
	
		class MpcFile;

		class PgmToProgramConverter
		{

		private:
			mpc::file::pgmreader::ProgramFileReader* reader{ nullptr };
			std::weak_ptr<mpc::sampler::Program> program{};
			bool done{ false };
			std::vector<std::string> soundNames{};

		public:
			void setSlider();
			void setNoteParameters();
			void setMixer();

		public:
			std::weak_ptr<mpc::sampler::Program> get();
			std::vector<std::string> getSoundNames();

			PgmToProgramConverter(MpcFile* file, std::weak_ptr<mpc::sampler::Sampler> sampler);
		};

	}
}
