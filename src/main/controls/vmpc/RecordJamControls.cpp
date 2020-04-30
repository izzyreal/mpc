#include <controls/vmpc/RecordJamControls.hpp>

#include <ui/vmpc/DirectToDiskRecorderGui.hpp>

using namespace mpc::controls::vmpc;
using namespace std;

RecordJamControls::RecordJamControls()
	: AbstractVmpcControls()
{
}

void RecordJamControls::function(int i)
{
	init();
	string outputFolder = "";
	int minutes;
	int lengthInFrames;
	bool split;
	switch (i) {
	case 3:
		ls.lock()->openScreen("directtodiskrecorder");
		break;
	case 4:
		outputFolder = d2dRecorderGui->getOutputfolder();
		minutes = 10;
		lengthInFrames = 44100 * 60 * minutes;
		split = d2dRecorderGui->isSplitLR();
		//thread(RecordJamControls_function_1(this, lengthInFrames, outputFolder, split, minutes))->start();
		ls.lock()->openScreen("sequencer");
		break;
	}
}
