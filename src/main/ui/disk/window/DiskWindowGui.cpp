#include <ui/disk/window/DiskWindowGui.hpp>

#include <sequencer/Sequence.hpp>

using namespace mpc::ui::disk::window;
using namespace std;

DiskWindowGui::DiskWindowGui()
{
}

void DiskWindowGui::setDelete(int i)
{
	if (i < 0 || i > 8) return;
	deleteIndex = i;
	setChanged();
	notifyObservers(string("delete"));
}

int DiskWindowGui::getDelete()
{
    return deleteIndex;
}

DiskWindowGui::~DiskWindowGui() {
}
