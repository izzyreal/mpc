#include <ui/Uis.hpp>

#include <ui/misc/PunchGui.hpp>
#include <ui/misc/SecondSeqGui.hpp>
#include <ui/misc/TransGui.hpp>

#include <ui/midisync/MidiSyncGui.hpp>

using namespace mpc::ui;
using namespace std;

Uis::Uis() 
{
	if (noteNames.size() != 128)
	{
		noteNames = vector<string>(128);
		int octave = -2;
		int noteCounter = 0;
		
		for (int j = 0; j < 128; j++)
		{
			string octaveString = to_string(octave);
		
			if (octave == -2)
			{
				octaveString = u8"\u00D2";
			}

			if (octave == -1)
			{
				octaveString = u8"\u00D3";
			}

			noteNames[j] = someNoteNames[noteCounter] + octaveString;
			noteCounter++;
			
			if (noteCounter == 12)
			{
				noteCounter = 0;
				octave++;
			}

		}
	}

	midiSyncGui = new mpc::ui::midisync::MidiSyncGui();

	punchGui = new mpc::ui::misc::PunchGui();
	transGui = new mpc::ui::misc::TransGui();
	secondSeqGui = new mpc::ui::misc::SecondSeqGui();
}

vector<string> Uis::noteNames;

mpc::ui::midisync::MidiSyncGui* Uis::getMidiSyncGui()
{
	return midiSyncGui;
}

mpc::ui::misc::PunchGui* Uis::getPunchGui()
{
	return punchGui;
}

mpc::ui::other::OthersGui* Uis::getOthersGui()
{
	return othersGui;
}

mpc::ui::misc::TransGui* Uis::getTransGui()
{
	return transGui;
}

mpc::ui::misc::SecondSeqGui* Uis::getSecondSeqGui()
{
	return secondSeqGui;
}

Uis::~Uis()
{
	if (midiSyncGui != nullptr) delete midiSyncGui;
	if (punchGui != nullptr) delete punchGui;
	if (transGui != nullptr) delete transGui;
	if (secondSeqGui != nullptr) delete secondSeqGui;
	if (othersGui != nullptr) delete othersGui;
}
