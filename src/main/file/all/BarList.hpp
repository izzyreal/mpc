#pragma once

#include <vector>

namespace mpc::sequencer
{
    class Sequence;
}

namespace mpc::file::all
{
    class Bar;

    class BarList
    {

    public:
        std::vector<Bar *> bars;
        std::vector<char> saveBytes;

        std::vector<Bar *> getBars();

        std::vector<char> &getBytes();
        BarList(const std::vector<char> &loadBytes);

        BarList(sequencer::Sequence *seq);
        ~BarList();
    };
} // namespace mpc::file::all
