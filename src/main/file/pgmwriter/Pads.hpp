#pragma once

#include <string>
#include <vector>

namespace mpc::sampler
{
    class Program;
}

namespace mpc::file::pgmwriter
{
    class Pads
    {

    private:
        std::vector<char> padsArray;
        static std::string fxBoardSettings; // dummy -- not implemented in any way -- just to fool anybody who's trying to read the product of this parser

    public:
        std::vector<char> getPadsArray();

    private:
        void setPadMidiNote(int pad, int padMidiNote);

    public:
        static std::vector<char> getFxBoardSettings();

        Pads(mpc::sampler::Program *program);
    };
} // namespace mpc::file::pgmwriter
