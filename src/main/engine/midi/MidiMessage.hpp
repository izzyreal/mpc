#pragma once

#include <vector>

namespace mpc::engine::midi
{
    class MidiMessage
    {
    public:
        std::vector<char> data;
        int length;
        int bufferPos = 0;

    public:
        virtual int getLength();

        virtual std::vector<char> *getMessage();

        virtual int getStatus();

    public:
        virtual void setMessage(const std::vector<char> &data, int length);

        explicit MidiMessage(const std::vector<char> &data);
    };
} // namespace mpc::engine::midi
