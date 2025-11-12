#pragma once

#include <vector>
#include <string>
#include <memory>

namespace mpc::sampler
{
    class Sound;
}

namespace mpc::file::sndwriter
{
    class SndHeaderWriter;
}

namespace mpc::file::sndwriter
{
    class SndWriter
    {

        static const int HEADER_SIZE = 42;
        std::shared_ptr<SndHeaderWriter> sndHeaderWriter;
        sampler::Sound *sound;
        std::vector<char> sndFileArray;

        void setValues();
        void setName(const std::string &s) const;
        void setMono(bool b) const;
        void setFramesCount(int i) const;
        void setSampleRate(int i) const;
        void setLevel(int i) const;
        void setStart(int i) const;
        void setEnd(int i) const;
        void setLoopLength(int i) const;
        void setLoopEnabled(bool b) const;
        void setTune(int i) const;
        void setBeatCount(int i) const;
        void setSampleData(const std::vector<float> &fa, bool b);

    public:
        std::vector<char> &getSndFileArray();

        SndWriter(sampler::Sound *sound);
    };
} // namespace mpc::file::sndwriter
