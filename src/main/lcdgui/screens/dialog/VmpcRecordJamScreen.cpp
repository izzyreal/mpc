#include "VmpcRecordJamScreen.hpp"

using namespace mpc::lcdgui::screens::dialog;
using namespace std;

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
		ls.lock()->openScreen("vmpc-direct-to-disk-recorder");
		break;
	case 4:
	{
		//auto outputFolder = d2dRecorderGui->getOutputfolder();
		//auto minutes = 10;
		//auto lengthInFrames = 44100 * 60 * minutes;
		//auto split = d2dRecorderGui->isSplitLR();
		//thread(VmpcRecordJamScreen_function_1(this, lengthInFrames, outputFolder, split, minutes))->start();
		ls.lock()->openScreen("sequencer");
		break;
	}
	}
}
