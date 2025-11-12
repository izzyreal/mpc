#pragma once

#include "engine/audio/server/AudioClient.hpp"

#include <vector>

namespace mpc::engine::audio::server
{

    class CompoundAudioClient final : public AudioClient
    {

        std::vector<AudioClient *> clients;

    public:
        void work(int nFrames) override;

        void add(AudioClient *client);

        void remove(const AudioClient *client);
    };

} // namespace mpc::engine::audio::server
