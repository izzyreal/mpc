#pragma once

#include <memory>
#include <vector>
#include <thread>

namespace mpc
{
    class Mpc;
}

namespace mpc::sampler
{
    class Sound;
}

namespace mpc::disk
{

    class SoundSaver
    {

        Mpc &mpc;
        std::vector<std::shared_ptr<sampler::Sound>> sounds;
        bool wav = false;
        std::thread saveSoundsThread;
        static void static_saveSounds(void *this_p);
        void saveSounds() const;

    public:
        SoundSaver(Mpc &mpc,
                   const std::vector<std::shared_ptr<sampler::Sound>> &sounds,
                   bool wav);
        ~SoundSaver();
    };
} // namespace mpc::disk
