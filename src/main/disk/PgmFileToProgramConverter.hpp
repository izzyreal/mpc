#pragma once

#include <memory>
#include <vector>
#include <string>
#include "mpc_types.hpp"

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
	class PgmFileToProgramConverter
	{

	private:
		std::shared_ptr<mpc::sampler::Program> program;
		std::vector<std::string> soundNames;

        static void setSlider(mpc::file::pgmreader::ProgramFileReader&, std::shared_ptr<mpc::sampler::Program>);
        static void setNoteParameters(mpc::file::pgmreader::ProgramFileReader&, std::shared_ptr<mpc::sampler::Program>);
        static void setMixer(mpc::file::pgmreader::ProgramFileReader&, std::shared_ptr<mpc::sampler::Program>);

    public:
		std::weak_ptr<mpc::sampler::Program> get();
		std::vector<std::string> getSoundNames();

        static program_or_error loadFromFileAndConvert(
                std::shared_ptr<MpcFile>,
                std::shared_ptr<mpc::sampler::Sampler>,
                const int replaceIndex,
                std::vector<std::string>& soundNames);

		PgmFileToProgramConverter(std::weak_ptr<MpcFile>, std::weak_ptr<mpc::sampler::Sampler> sampler, const int replaceIndex);
	};
}
