#include "DeleteAllTracksControls.hpp"

#include <sequencer/Sequence.hpp>

using namespace mpc::controls::sequencer::dialog;
using namespace std;

DeleteAllTracksControls::DeleteAllTracksControls()
	: AbstractSequencerControls()
{
}

void DeleteAllTracksControls::function(int i)
{
	init();
	switch (i) {
	case 3:
		ls.lock()->openScreen("deletetrack");
		break;
	case 4:
		sequence.lock()->purgeAllTracks();
		ls.lock()->openScreen("sequencer");
		break;
	}
}
