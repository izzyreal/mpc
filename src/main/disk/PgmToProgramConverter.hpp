#pragma once

#include <memory>
#include <vector>
#include <string>

namespace mpc::sampler {
	class Sampler;
	class Program;
}

namespace mpc::file::pgmreader {
	class ProgramFileReader;
}

namespace mpc::disk {
	class MpcFile;
}

namespace mpc::disk {
	class PgmToProgramConverter
	{

	private:
		mpc::file::pgmreader::ProgramFileReader* reader{ nullptr };
		std::weak_ptr<mpc::sampler::Program> program;
		bool done{ false };
		std::vector<std::string> soundNames;

	public:
		void setSlider();
		void setNoteParameters();
		void setMixer();

	public:
		std::weak_ptr<mpc::sampler::Program> get();
		std::vector<std::string> getSoundNames();

		PgmToProgramConverter(std::weak_ptr<MpcFile>, std::weak_ptr<mpc::sampler::Sampler> sampler, const int replaceIndex);
		~PgmToProgramConverter();
	};
}
