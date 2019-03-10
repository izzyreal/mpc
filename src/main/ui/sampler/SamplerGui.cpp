#include <ui/sampler/SamplerGui.hpp>

#include <Mpc.hpp>
#include <hardware/Hardware.hpp>
#include <hardware/Led.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::ui::sampler;
using namespace std;

SamplerGui::SamplerGui(mpc::Mpc* mpc) 
{
	this->mpc = mpc;
}

void SamplerGui::setPadAndNote(int pad, int note)
{
    if (pad < -1 || pad > 63 || note < 34 | note > 98) return;
    if (prevPad != pad && pad != -1) prevPad = pad;

    this->pad = pad;
    if (note != 34) prevNote = note;
    this->note = note;
    setChanged();
    notifyObservers(string("padandnote"));
}

int SamplerGui::getNote()
{
    return note;
}

int SamplerGui::getPad()
{
    return pad;
}

void SamplerGui::setPadAssignMaster(bool b)
{
    if (padAssignMaster == b)
        return;

    padAssignMaster = b;
    setChanged();
    notifyObservers(string("padassignmode"));
}

bool SamplerGui::isPadAssignMaster()
{
    return padAssignMaster;
}

void SamplerGui::setBank(int i)
{
	if (i == bank) return;
	if (i < 0 || i > 3) return;

	bank = i;
	auto hw = mpc->getHardware().lock();
	hw->getLed("padbanka").lock()->light(i == 0);
	hw->getLed("padbankb").lock()->light(i == 1);
	hw->getLed("padbankc").lock()->light(i == 2);
	hw->getLed("padbankd").lock()->light(i == 3);
	setChanged();
	notifyObservers(string("bank"));
}

int SamplerGui::getBank()
{
    return bank;
}

void SamplerGui::setSelectedDrum(int i)
{
    if (i < 0 || i > 3)
        return;

    selectedDrum = i;
    setChanged();
    notifyObservers(string("drum"));
}

int SamplerGui::getSelectedDrum()
{
    return selectedDrum;
}

string SamplerGui::getPrevScreenName()
{
    return prevScreenName;
}

void SamplerGui::setPrevScreenName(string s)
{
    prevScreenName = s;
}

int SamplerGui::getPrevNote()
{
    return prevNote;
}

int SamplerGui::getPrevPad()
{
    return prevPad;
}

bool SamplerGui::isPadToIntSound()
{
    return padToIntSound;
}

void SamplerGui::setPadToIntSound(bool b)
{
    padToIntSound = b;
    setChanged();
    notifyObservers(string("padtointsound"));
}

int SamplerGui::getInput()
{
    return input;
}

void SamplerGui::setInput(int i)
{
    if(i < 0 || i > 1)
        return;

    input = i;
    setChanged();
    notifyObservers(string("input"));
}

int SamplerGui::getThreshold()
{
    return threshold;
}

void SamplerGui::setThreshold(int i)
{
    if(i < -int (64) || i > 0)
        return;

    threshold = i;
    setChanged();
    notifyObservers(string("threshold"));
}

int SamplerGui::getMode()
{
    return mode;
}

void SamplerGui::setMode(int i)
{
    if(i < 0 || i > 2)
        return;

    mode = i;
    setChanged();
    notifyObservers(string("mode"));
}

int SamplerGui::getTime()
{
    return time;
}

void SamplerGui::setTime(int i)
{
    if(i < 0 || i > 3786)
        return;

    time = i;
    setChanged();
    notifyObservers(string("time"));
}

int SamplerGui::getMonitor()
{
    return monitor;
}

void SamplerGui::setMonitor(int i)
{
    if(i < 0 || i > 5)
        return;

    monitor = i;
    setChanged();
    notifyObservers(string("monitor"));
}

int SamplerGui::getPreRec()
{
    return preRec;
}

void SamplerGui::setPreRec(int i)
{
    if(i < 0 || i > 100)
        return;

    preRec = i;
    setChanged();
    notifyObservers(string("prerec"));
}

void SamplerGui::setNewName(string newName)
{
    this->newName = newName;
}

string SamplerGui::getNewName()
{
    return newName;
}

void SamplerGui::notify(string str)
{
    setChanged();
    notifyObservers(str);
}

SamplerGui::~SamplerGui() {
}
