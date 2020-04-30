#include "VmpcSettingsObserver.hpp"

#include <lcdgui/Field.hpp>

using namespace mpc::ui::vmpc;
using namespace std;

VmpcSettingsObserver::VmpcSettingsObserver()
	: gui(VmpcSettingsGui::instance())
{
	gui.addObserver(this);
}

void VmpcSettingsObserver::update(moduru::observer::Observable* o, nonstd::any a)
{
	displayInitialPadMapping();
}

void VmpcSettingsObserver::displayInitialPadMapping()
{
}

VmpcSettingsObserver::~VmpcSettingsObserver() {
	gui.deleteObserver(this);
}
