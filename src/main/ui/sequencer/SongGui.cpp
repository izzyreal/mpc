#include <ui/sequencer/SongGui.hpp>

using namespace mpc::ui::sequencer;
using namespace std;

SongGui::SongGui()
{
	offset = -1;
	defaultSongName = "Song";
}

void SongGui::setOffset(int i)
{
    if(i < -1) return;
    offset = i;
    setChanged();
    notifyObservers(string("offset"));
}

int SongGui::getOffset()
{
    return offset;
}

int SongGui::getSelectedSongIndex()
{
    return selectedSongIndex;
}

void SongGui::setSelectedSongIndex(int i)
{
    if(i < 0 || i > 19) return;
    selectedSongIndex = i;
    setChanged();
    notifyObservers(string("selectedsongindex"));
}

void SongGui::setDefaultSongName(string s)
{
    defaultSongName = s;
}

string SongGui::getDefaultSongName()
{
    return defaultSongName;
}

void SongGui::setLoop(bool b)
{
    loop = b;
    setChanged();
    notifyObservers(string("loop"));
}

bool SongGui::isLoopEnabled()
{
    return loop;
}