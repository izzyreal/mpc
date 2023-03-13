#pragma once

#include <engine/audio/core/ChannelFormat.hpp>

namespace ctoot {

    namespace control {
        class EnumControl;
    }

    namespace audio::mixer {

        struct MixVariables
        {
        public:
            virtual std::string getName() = 0;

            virtual bool isEnabled() = 0;

            virtual bool isMaster() = 0;

            virtual float getGain() = 0;

            virtual void getChannelGains(std::vector<float> *dest) = 0;

            virtual std::shared_ptr<ctoot::audio::core::ChannelFormat> getChannelFormat() = 0;

            virtual float getSmoothingFactor() = 0;

            virtual ctoot::control::EnumControl *getRouteControl() { return nullptr; }
        };
    }
}
