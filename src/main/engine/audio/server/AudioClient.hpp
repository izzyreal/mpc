#pragma once

namespace mpc::engine::audio::server
{
    struct AudioClient
    {
        virtual ~AudioClient() = default;
        virtual void work(int nFrames) = 0;
    };
} // namespace mpc::engine::audio::server
