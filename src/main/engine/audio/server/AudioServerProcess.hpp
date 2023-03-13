#pragma once

#include <engine/audio/server/IOAudioProcess.hpp>

#include <string>

namespace ctoot::audio {

    namespace core {
        class ChannelFormat;
    }

    namespace server {

        class AudioServerProcess
                : public IOAudioProcess
        {

        private:
            std::string name;

        protected:
            std::shared_ptr<ctoot::audio::core::ChannelFormat> format;

        public:
            std::string getName() override;

            std::shared_ptr<ctoot::audio::core::ChannelFormat> getChannelFormat() override;

            AudioServerProcess(std::string name);
        };

    }
}
