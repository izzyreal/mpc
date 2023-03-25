#pragma once

#include <string>
#include <vector>

namespace mpc::engine::control {
    class EnumControl;
}

namespace mpc::engine::audio::mixer {

    struct MixVariables
    {
    public:
        virtual std::string getName() = 0;

        virtual bool isEnabled() = 0;

        virtual bool isMaster() = 0;

        virtual float getGain() = 0;

        virtual void getChannelGains(std::vector<float> *dest) = 0;

        virtual float getSmoothingFactor() = 0;

        virtual mpc::engine::control::EnumControl *getRouteControl()
        { return nullptr; }
    };
}
