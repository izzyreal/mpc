#include <ui/sampler/window/SamplerWindowGui.hpp>

#include <lcdgui/LayeredScreen.hpp>

using namespace mpc::ui::sampler::window;
using namespace std;

SamplerWindowGui::SamplerWindowGui()
{
}

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

SamplerWindowGui::~SamplerWindowGui() {
}
