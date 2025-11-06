#pragma once

#include "engine/audio/server/AudioClient.hpp"

#include <vector>

namespace mpc::engine::audio::server
{

    class CompoundAudioClient final : public AudioClient
    {

    private:
        std::vector<AudioClient *> clients;

    public:
        void work(int nFrames) override;

        void add(AudioClient *client);

        void remove(AudioClient *client);
    };

} // namespace mpc::engine::audio::server
