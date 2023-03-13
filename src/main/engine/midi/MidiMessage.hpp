#pragma once

#include <vector>

namespace ctoot::midi::core {
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
}
