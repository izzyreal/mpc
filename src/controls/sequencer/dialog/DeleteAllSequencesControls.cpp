#include "DeleteAllSequencesControls.hpp"

#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::sequencer::dialog;
using namespace std;

DeleteAllSequencesControls::DeleteAllSequencesControls(mpc::Mpc* mpc)
	: AbstractSequencerControls(mpc)
{
}

void DeleteAllSequencesControls::function(int i)
{
	init();
	switch (i) {
	case 3:
		ls.lock()->openScreen("deletesequence");
		break;
	case 4:
		sequencer.lock()->purgeAllSequences();
		ls.lock()->openScreen("sequencer");
		break;
	}
}
