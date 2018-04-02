#include <ui/sampler/window/EditSoundGui.hpp>

using namespace mpc::ui::sampler::window;
using namespace std;

EditSoundGui::EditSoundGui()
{
}

void EditSoundGui::setNewName(string s)
{
    newName = s;
}

string EditSoundGui::getNewName()
{
    return newName;
}

void EditSoundGui::setEdit(int i)
{
    if (i < 0 || i > 8) return;
    if (previousScreenName.compare("zone") != 0 && i > 7) return;
    edit = i;
    setChanged();
    notifyObservers(string("edit"));
}

int EditSoundGui::getEdit()
{
    return edit;
}

int EditSoundGui::getInsertSndNr()
{
    return insertSoundNumber;
}

void EditSoundGui::setInsertSndNr(int i, int soundCount)
{
	if (i < 0 || i > soundCount - 1) return;
    insertSoundNumber = i;
    setChanged();
    notifyObservers(string("insertsoundnumber"));
}

void EditSoundGui::setTimeStretchRatio(int i)
{
	if (i < 5000 || i > 20000) return;
    timeStretchRatio = i;
    setChanged();
    notifyObservers(string("timestretchratio"));
}

int EditSoundGui::getTimeStretchRatio()
{
    return timeStretchRatio;
}

void EditSoundGui::setTimeStretchPresetNumber(int i)
{
    if(i < 0 || i > 53) return;

    timeStretchPresetNumber = i;
    setChanged();
    notifyObservers(string("timestretchpresetnumber"));
}

int EditSoundGui::getTimeStretchPresetNumber()
{
    return timeStretchPresetNumber;
}

void EditSoundGui::setTimeStretchAdjust(int i)
{
    if(i < -100 || i > 100) return;
    timeStretchAdjust = i;
    setChanged();
    notifyObservers(string("timestretchadjust"));
}

int EditSoundGui::getTimeStretchAdjust()
{
    return timeStretchAdjust;
}

void EditSoundGui::setPreviousScreenName(string s)
{
    previousScreenName = s;
}

string EditSoundGui::getPreviousScreenName()
{
    return previousScreenName;
}

void EditSoundGui::setNewName(int i, string s)
{
	newNames.insert(newNames.begin() + i, s);
}

string EditSoundGui::getNewName(int i)
{
    return newNames[i];
}

bool EditSoundGui::getCreateNewProgram()
{
    return createNewProgram;
}

void EditSoundGui::setCreateNewProgram(bool b)
{
    createNewProgram = b;
    setChanged();
    notifyObservers(string("createnewprogram"));
}

int EditSoundGui::getEndMargin()
{
    return endMargin;
}

void EditSoundGui::setEndMargin(int i)
{
    if(i < 0 || i > 99) return;
    endMargin = i;
    setChanged();
    notifyObservers(string("endmargin"));
}

EditSoundGui::~EditSoundGui() {
}
