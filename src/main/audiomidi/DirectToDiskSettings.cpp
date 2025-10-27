#include "DirectToDiskSettings.hpp"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <utility>

using namespace mpc::audiomidi;

DirectToDiskSettings::DirectToDiskSettings(
    const int lengthInFramesToUse,
    const bool splitStereoIntoLeftAndRightChannelToUse,
    const int sampleRateToUse, std::string recordingNameToUse)
    : lengthInFrames(lengthInFramesToUse),
      splitStereoIntoLeftAndRightChannel(
          splitStereoIntoLeftAndRightChannelToUse),
      sampleRate(sampleRateToUse), recordingName(std::move(recordingNameToUse))
{
}

std::string DirectToDiskSettings::getTimeStamp()
{
    std::time_t t = std::time(nullptr);
    std::tm *tm = std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(tm, "%d-%b-%Y-%H-%M-%S");
    return oss.str();
}
