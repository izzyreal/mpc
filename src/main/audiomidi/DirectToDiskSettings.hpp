#pragma once

#include <string>

namespace mpc::audiomidi
{

    class DirectToDiskSettings
    {

    public:
        const int sampleRate;
        const int lengthInFrames;
        const bool splitStereoIntoLeftAndRightChannel;
        const std::string recordingName;

        static std::string getTimeStamp();

    public:
        DirectToDiskSettings(int lengthInFrames,
                             bool splitStereoIntoLeftAndRightChannel,
                             int sampleRate,
                             std::string recordingName);
    };
} // namespace mpc::audiomidi
