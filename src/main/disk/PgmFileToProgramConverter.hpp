#pragma once

#include <memory>
#include <vector>
#include <string>
#include "mpc_types.hpp"

namespace mpc
{
    class Mpc;
}

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
        loadFromFileAndConvert(mpc::Mpc &, const std::shared_ptr<MpcFile> &,
                               std::shared_ptr<sampler::Program>,
                               std::vector<std::string> &soundNames);

    private:
        static void setSlider(const file::pgmreader::ProgramFileReader &,
                              const std::shared_ptr<sampler::Program> &);
        static void
        setNoteParameters(mpc::Mpc &,
                          const file::pgmreader::ProgramFileReader &,
                          const std::shared_ptr<sampler::Program> &);
        static void setMixer(const file::pgmreader::ProgramFileReader &,
                             const std::shared_ptr<sampler::Program> &);
    };
} // namespace mpc::disk
