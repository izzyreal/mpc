#include "Assign16LevelsGui.hpp"

using namespace mpc::ui::sequencer::window;
using namespace std;

void Assign16LevelsGui::setNote(int newNote)
{
    if (newNote < 35 || newNote > 98)
    {
        return;
    }

    note = newNote;
    setChanged();
    notifyObservers(string("note"));
}

int Assign16LevelsGui::getNote()
{
	return note;
}

int Assign16LevelsGui::getParameter()
{
    return parameter;
}

void Assign16LevelsGui::setParameter(int i)
{
    if (i < 0 || i > 1)
    {
        return;
    }

    parameter = i;
    setChanged();
    notifyObservers(string("parameter"));
}

int Assign16LevelsGui::getType()
{
    return type;
}

void Assign16LevelsGui::setType(int i)
{
    if (i < 0 || i > 3)
    {
        return;
    }

    type = i;
    setChanged();
    notifyObservers(string("type"));
}

int Assign16LevelsGui::getOriginalKeyPad()
{
    return originalKeyPad;
}

void Assign16LevelsGui::setOriginalKeyPad(int i)
{
    if (i < 0 || i > 15)
    {
        return;
    }

    originalKeyPad = i;
    setChanged();
    notifyObservers(string("originalkeypad"));
}
