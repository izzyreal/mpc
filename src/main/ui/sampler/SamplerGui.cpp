#include "SamplerGui.hpp"

#include <Mpc.hpp>
#include <hardware/Hardware.hpp>
#include <hardware/Led.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::ui::sampler;
using namespace std;

void SamplerGui::setPadAndNote(int pad, int note)
{
    if (pad < -1 || pad > 63 || note < 34 || note > 98)
    {
        return;
    }

    if (prevPad != pad && pad != -1)
    {
        prevPad = pad;
    }

    this->pad = pad;
    
    if (note != 34) {
        prevNote = note;
    }

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
	auto hw = Mpc::instance().getHardware().lock();
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
