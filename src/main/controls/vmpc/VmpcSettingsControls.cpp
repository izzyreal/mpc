#include "VmpcSettingsControls.hpp"

#include <ui/vmpc/VmpcSettingsGui.hpp>

#include <Mpc.hpp>

using namespace mpc::controls::vmpc;
using namespace std;

void VmpcSettingsControls::turnWheel(int i)
{
    init();
	if (param.compare("initial-pad-mapping") == 0) {
		auto gui = &mpc::ui::vmpc::VmpcSettingsGui::instance();
		gui->setInitialPadMapping(gui->getInitialPadMapping() + i);
	}
}
