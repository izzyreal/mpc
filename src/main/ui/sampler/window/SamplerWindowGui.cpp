#include <ui/sampler/window/SamplerWindowGui.hpp>

#include <lcdgui/LayeredScreen.hpp>

using namespace mpc::ui::sampler::window;
using namespace std;

SamplerWindowGui::SamplerWindowGui()
{
}
vector<string> SamplerWindowGui::padFocusNames = vector<string>{ "a3", "b3", "c3", "d3", "a2", "b2", "c2", "d2", "a1", "b1", "c1", "d1", "a0", "b0", "c0", "d0" };

int SamplerWindowGui::getDeletePgm()
{
    return deletePgm;
}

void SamplerWindowGui::setDeletePgm(int i, int programCount)
{
    if (i < 0 || i > programCount - 1)
    {
        return;
    }
	deletePgm = i;
	setChanged();
	notifyObservers(string("deletepgm"));
}

int SamplerWindowGui::getPgm0()
{
    return pgm0;
}

void SamplerWindowGui::setPgm0(int i, int programCount)
{
	if (i < 0 || i > programCount - 1) return;
    this->pgm0 = i;
    setChanged();
    notifyObservers(string("pgm0"));
}

int SamplerWindowGui::getPgm1()
{
    return pgm1;
}

void SamplerWindowGui::setPgm1(int i, int programCount)
{
	if (i < 0 || i > programCount - 1) return;
    pgm1 = i;
    setChanged();
    notifyObservers(string("pgm1"));
}

string SamplerWindowGui::getNewName()
{
    return newName;
}

void SamplerWindowGui::setNewName(string s)
{
    newName = s;
}

int SamplerWindowGui::getNewProgramChange()
{
    return newProgramChange;
}

void SamplerWindowGui::setNewProgramChange(int i)
{
    if (i < 1 || i > 128) return;
    newProgramChange = i;
    setChanged();
    notifyObservers(string("newprogramchange"));
}

int SamplerWindowGui::getPadNumberFromFocus(const string& focus, int bank)
{
	auto pn = -1;
	for (int i = 0; i < SamplerWindowGui::padFocusNames.size(); i++) {
		if (SamplerWindowGui::padFocusNames[i].compare(focus) == 0) {
			pn = i;
			break;
		}
	}
	pn += bank * 16;
	return pn;
}

string SamplerWindowGui::getFocusFromPadNumber(const int& padNumber) // feed it mpc.getPadNumber()
{
	int padNr = padNumber;
	while (padNr > 15) padNr -= 16;
	return padFocusNames[padNr];
}

int SamplerWindowGui::getProg0()
{
    return prog0;
}

void SamplerWindowGui::setProg0(int i, const int& programCount)
{
	if (i < 0 || i > programCount - 1) return;
    prog0 = i;
    setChanged();
    notifyObservers(string("prog0"));
}

int SamplerWindowGui::getProg1()
{
    return prog1;
}

void SamplerWindowGui::setProg1(int i, const int& programCount)
{
	if (i < 0 || i > programCount - 1) return;
    prog1 = i;
    setChanged();
    notifyObservers(string("prog1"));
}

int SamplerWindowGui::getNote0()
{
    return note0;
}

void SamplerWindowGui::setNote0(int i)
{
    if (i < 0 || i > 63) return;
    note0 = i;
    setChanged();
    notifyObservers(string("note0"));
}

int SamplerWindowGui::getNote1()
{
    return note1;
}

void SamplerWindowGui::setNote1(int i)
{
    if(i < 0 || i > 63) return;
    note1 = i;
    setChanged();
    notifyObservers(string("note1"));
}

int SamplerWindowGui::getAutoChromAssSnd()
{
    return autoChromAssSnd;
}

void SamplerWindowGui::setAutoChromAssSnd(int i, const int& soundCount)
{
	if (i < -1 || i > soundCount -1) return;
    autoChromAssSnd = i;
    setChanged();
    notifyObservers(string("autochromasssnd"));
}

int SamplerWindowGui::getTune()
{
    return tune;
}

void SamplerWindowGui::setTune(int i)
{
    if (i < -240 || i > 240) return;
    tune = i;
    setChanged();
    notifyObservers(string("tune"));
}

int SamplerWindowGui::getOriginalKey()
{
    return originalKey;
}

void SamplerWindowGui::setOriginalKey(int i)
{
	if (i < 35 || i > 98) return;
	originalKey = i;
	setChanged();
	notifyObservers(string("originalkey"));
}

SamplerWindowGui::~SamplerWindowGui() {
}
