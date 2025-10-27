#include "VmpcRecordJamScreen.hpp"

#include "audiomidi/AudioMidiServices.hpp"
#include "audiomidi/DirectToDiskSettings.hpp"

#include "engine/audio/server/NonRealTimeAudioServer.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::audiomidi;

VmpcRecordJamScreen::VmpcRecordJamScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "vmpc-record-jam", layerIndex)
{
}

void VmpcRecordJamScreen::function(int i)
{

    switch (i)
    {
        case 3:
            mpc.getLayeredScreen()
                ->openScreen<VmpcDirectToDiskRecorderScreen>();
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
                mpc.getLayeredScreen()->openScreen<SequencerScreen>();
                ams->startBouncing();
            }
            else
            {
                mpc.getLayeredScreen()->openScreen<VmpcFileInUseScreen>();
            }

            break;
        }
    }
}
