#include <ui/sequencer/SequencerGui.hpp>

using namespace mpc::ui::sequencer;
using namespace std;

SequencerGui::SequencerGui()
{
	note = 35;
	originalKeyPad = 0;
}

int SequencerGui::getNote()
{
    return note;
}

void SequencerGui::setNote(int i)
{
    if(i < 35 || i > 98)
        return;

    note = i;
	setChanged();
	notifyObservers(string("note"));
}

int SequencerGui::getParameter()
{
    return parameter;
}

void SequencerGui::setParameter(int i)
{
    if(i < 0 || i > 1)
        return;

    parameter = i;
	setChanged();
	notifyObservers(string("parameter"));
}

int SequencerGui::getType()
{
    return type;
}

void SequencerGui::setType(int i)
{
    if(i < 0 || i > 3)
        return;

    type = i;
	setChanged();
	notifyObservers(string("type"));
}

int SequencerGui::getOriginalKeyPad()
{
    return originalKeyPad;
}

void SequencerGui::setOriginalKeyPad(int i)
{
    if(i < 0 || i > 15)
        return;

    originalKeyPad = i;
	setChanged();
	notifyObservers(string("originalkeypad"));
}
