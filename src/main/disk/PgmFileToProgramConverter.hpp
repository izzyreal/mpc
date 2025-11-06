#pragma once

#include <memory>
#include <vector>
#include <string>
#include "mpc_types.hpp"

namespace mpc::sampler
{
    class Sampler;
    class Program;
} // namespace mpc::sampler

namespace mpc::file::pgmreader
{
    class ProgramFileReader;
}

namespace mpc::disk
{
    class MpcFile;
}

namespace mpc::disk
{
    class PgmFileToProgramConverter
    {
    public:
        static program_or_error
        loadFromFileAndConvert(const std::shared_ptr<MpcFile> &,
                               std::shared_ptr<mpc::sampler::Program>,
                               std::vector<std::string> &soundNames);

    private:
        static void setSlider(mpc::file::pgmreader::ProgramFileReader &,
                              const std::shared_ptr<mpc::sampler::Program> &);
        static void
        setNoteParameters(mpc::file::pgmreader::ProgramFileReader &,
                          const std::shared_ptr<mpc::sampler::Program> &);
        static void setMixer(mpc::file::pgmreader::ProgramFileReader &,
                             const std::shared_ptr<mpc::sampler::Program> &);
    };
} // namespace mpc::disk
