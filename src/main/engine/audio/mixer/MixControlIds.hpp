#pragma once

#include <cstdint>

namespace ctoot::audio::mixer {

    struct MixControlIds
    {
        static const int GAIN{1};
        static const int MUTE{2};
        static const int ROUTE{4};
        static const int LCR{5};
    };

}
