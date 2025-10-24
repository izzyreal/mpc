#pragma once

#include <memory>
#include <vector>

namespace mpc::sampler
{
    class Sampler;
    class Program;
} // namespace mpc::sampler

namespace mpc::file::pgmwriter
{

    class PgmWriter
    {

    private:
        std::vector<char> header;
        std::vector<char> pgmName;
        std::vector<char> sampleNames;
        std::vector<char> slider;
        std::vector<char> midiNotes;
        std::vector<char> mixer;
        std::vector<char> pads;

    public:
        std::vector<char> get();

        PgmWriter(mpc::sampler::Program *program, std::weak_ptr<mpc::sampler::Sampler> sampler);
    };
} // namespace mpc::file::pgmwriter
