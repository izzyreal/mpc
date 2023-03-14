#pragma once

#include <cstdint>

namespace mpc::engine::audio::mixer {

    struct MixerControlsIds
    {
        static const int CHANNEL_STRIP{120};
        static const int GROUP_STRIP{121};
        static const int MAIN_STRIP{123};
        static const int AUX_STRIP{124};
        static const int AUX_BUS{126};
        static const int MAIN_BUS{127};
    };
}
