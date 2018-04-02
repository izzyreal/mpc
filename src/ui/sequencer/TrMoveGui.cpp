#include <ui/sequencer/TrMoveGui.hpp>

#include <sequencer/Sequence.hpp>

using namespace mpc::ui::sequencer;
using namespace std;

TrMoveGui::TrMoveGui() 
{
}

bool TrMoveGui::isSelected()
{
	if (selectedTrackIndex != -1) return true;
	return false;
}

int TrMoveGui::getSq()
{
    return sq;
}

void TrMoveGui::goUp()
{
	if (currentTrackIndex == 0) return;
	currentTrackIndex--;
	setChanged();
	notifyObservers(string("trmove"));
}

void TrMoveGui::goDown()
{
	if (currentTrackIndex == 63) return;
	currentTrackIndex++;
	setChanged();
	notifyObservers(string("trmove"));
}

int TrMoveGui::getCurrentTrackIndex()
{
    return currentTrackIndex;
}

void TrMoveGui::setSq(int i)
{
    if(i < 0 || i > 98) return;
    sq = i;
    setChanged();
    notifyObservers(string("sq"));
}

int TrMoveGui::getSelectedTrackIndex()
{
    return selectedTrackIndex;
}

void TrMoveGui::select()
{
    selectedTrackIndex = currentTrackIndex;
    setChanged();
    notifyObservers(string("trmove"));
}

void TrMoveGui::cancel()
{
    selectedTrackIndex = -1;
    setChanged();
    notifyObservers(string("trmove"));
}

void TrMoveGui::insert(mpc::sequencer::Sequence* s)
{
    s->moveTrack(selectedTrackIndex, currentTrackIndex);
    selectedTrackIndex = -1;
    setChanged();
    notifyObservers(string("trmove"));
}
