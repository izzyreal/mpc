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

    private:
        static const int HEADER_SIZE = 42;
        std::shared_ptr<SndHeaderWriter> sndHeaderWriter;
        mpc::sampler::Sound *sound;
        std::vector<char> sndFileArray;

    private:
        void setValues();
        void setName(std::string s);
        void setMono(bool b);
        void setFramesCount(int i);
        void setSampleRate(int i);
        void setLevel(int i);
        void setStart(int i);
        void setEnd(int i);
        void setLoopLength(int i);
        void setLoopEnabled(bool b);
        void setTune(int i);
        void setBeatCount(int i);
        void setSampleData(const std::vector<float> &fa, bool b);

    public:
        std::vector<char> &getSndFileArray();

        SndWriter(mpc::sampler::Sound *sound);
    };
} // namespace mpc::file::sndwriter
