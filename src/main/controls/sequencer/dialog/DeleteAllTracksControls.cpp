#include "DeleteAllTracksControls.hpp"

#include <sequencer/Sequence.hpp>

using namespace mpc::controls::sequencer::dialog;
using namespace std;

DeleteAllTracksControls::DeleteAllTracksControls(mpc::Mpc* mpc)
	: AbstractSequencerControls(mpc)
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
