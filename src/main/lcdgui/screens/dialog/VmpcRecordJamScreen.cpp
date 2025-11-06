#include "VmpcRecordJamScreen.hpp"

#include "Mpc.hpp"
#include "audiomidi/AudioMidiServices.hpp"
#include "audiomidi/DirectToDiskSettings.hpp"

#include "engine/audio/server/NonRealTimeAudioServer.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::audiomidi;

VmpcRecordJamScreen::VmpcRecordJamScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "vmpc-record-jam", layerIndex)
{
}

void VmpcRecordJamScreen::function(const int i)
{
    switch (i)
    {
        case 3:
            openScreenById(ScreenId::VmpcDirectToDiskRecorderScreen);
            break;
        case 4:
        {
            const auto minutes = 60;
            const auto ams = mpc.getAudioMidiServices();
            const auto rate =
                static_cast<int>(ams->getAudioServer()->getSampleRate());
            const auto lengthInFrames = rate * 60 * minutes;
            const auto recordingName =
                "Jam-" + DirectToDiskSettings::getTimeStamp();
            const auto settings = std::make_unique<DirectToDiskSettings>(
                lengthInFrames, false, rate, recordingName);

            if (ams->prepareBouncing(settings.get()))
            {
                openScreenById(ScreenId::SequencerScreen);
                ams->startBouncing();
            }
            else
            {
                openScreenById(ScreenId::VmpcFileInUseScreen);
            }

            break;
        }
    }
}
