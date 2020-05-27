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
