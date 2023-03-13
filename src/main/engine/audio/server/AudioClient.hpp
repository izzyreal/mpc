#pragma once

#include <cstdint>

namespace ctoot::audio::server {
    struct AudioClient
    {
        virtual void work(int nFrames) = 0;
    };
}
