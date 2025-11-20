#pragma once

#include "MpcSpecs.hpp"

namespace mpc::performance
{
    struct Event
    {

    };

    struct Track
    {

    };

    struct Sequence
    {
        std::array<Track, Mpc2000XlSpecs::TRACK_COUNT> tracks;
    };
}