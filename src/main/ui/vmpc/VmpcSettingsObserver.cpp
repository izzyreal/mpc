#include "VmpcSettingsObserver.hpp"

#include <Mpc.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Field.hpp>

using namespace mpc::ui::vmpc;
using namespace std;

VmpcSettingsObserver::VmpcSettingsObserver()
	: gui(VmpcSettingsGui::instance())
{
	gui.addObserver(this);
	displayInitialPadMapping();
}

void VmpcSettingsObserver::update(moduru::observer::Observable* o, nonstd::any a)
{
	displayInitialPadMapping();
}

void VmpcSettingsObserver::displayInitialPadMapping()
{
	auto textField = mpc::Mpc::instance().getLayeredScreen().lock()->lookupField("initial-pad-mapping").lock();
	textField->setText(initialPadMappingNames[gui.getInitialPadMapping()]);
}

VmpcSettingsObserver::~VmpcSettingsObserver() {
	gui.deleteObserver(this);
}
