#include "VmpcRecordJamScreen.hpp"

#include <lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp>
#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/DirectToDiskSettings.hpp>

#include <audio/server/NonRealTimeAudioServer.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::audiomidi;

VmpcRecordJamScreen::VmpcRecordJamScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "vmpc-record-jam", layerIndex)
{
}

void VmpcRecordJamScreen::function(int i)
{
	init();

	switch (i)
	{
	case 3:
		openScreen("vmpc-direct-to-disk-recorder");
		break;
	case 4:
	{
		auto minutes = 60;
		auto lengthInFrames = 44100 * 60 * minutes;
		auto vmpcDirectToDiskRecorderScreen = mpc.screens->get<VmpcDirectToDiskRecorderScreen>("vmpc-direct-to-disk-recorder");
		auto ams = mpc.getAudioMidiServices();
		auto rate = ams->getAudioServer()->getSampleRate();
		auto settings = std::make_unique<DirectToDiskSettings>(lengthInFrames, vmpcDirectToDiskRecorderScreen->outputFolder, false, rate);

		if (ams->prepareBouncing(settings.get()))
		{
			openScreen("sequencer");
			ams->startBouncing();
		}
		else
		{
			openScreen("vmpc-file-in-use");
		}

		break;
	}
	}
}
