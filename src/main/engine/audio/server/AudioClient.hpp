#pragma once

#include <memory>

namespace mpc::engine::audio::server
{
    struct AudioClient : public std::enable_shared_from_this<AudioClient>
    {
        virtual ~AudioClient() = default;
        virtual void work(int nFrames) = 0;
    };
} // namespace mpc::engine::audio::server
