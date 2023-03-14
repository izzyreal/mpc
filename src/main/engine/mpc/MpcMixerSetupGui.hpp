#pragma once

#include <memory>

namespace ctoot::mpc {
    class MpcMixerSetupGui
    {
    public:
        virtual bool isStereoMixSourceDrum() = 0;

        virtual bool isIndivFxSourceDrum() = 0;

    };
}
