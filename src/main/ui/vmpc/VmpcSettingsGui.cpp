#include <ui/vmpc/VmpcSettingsGui.hpp>

using namespace mpc::ui::vmpc;
using namespace std;

VmpcSettingsGui::VmpcSettingsGui()
{
}

VmpcSettingsGui::~VmpcSettingsGui()
{
}

int VmpcSettingsGui::getInitialPadMapping()
{
	return initialPadMapping;
}

void VmpcSettingsGui::setInitialPadMapping(int i)
{
	if (i < 0 || i > 1) {
		return;
	}

	initialPadMapping = i;

	setChanged();
	notifyObservers(string("initialPadMapping"));
}
