#include <controls/vmpc/VmpcDiskControls.hpp>

#include <Mpc.hpp>
#include <ui/vmpc/DeviceGui.hpp>

using namespace mpc::controls::vmpc;
using namespace std;

VmpcDiskControls::VmpcDiskControls(mpc::Mpc* mpc)
	: AbstractDiskControls(mpc)
{
}

void VmpcDiskControls::function(int i)
{
	init();
	switch (i) {
	case 0:
		ls.lock()->openScreen("audio");
		break;
	case 1:
		ls.lock()->openScreen("midi");
		break;
	}
}

void VmpcDiskControls::turnWheel(int i)
{
    init();
	if (param.compare("scsi") == 0) {
		deviceGui->setScsi(deviceGui->getScsi() + i);
	}
	else if (param.compare("accesstype") == 0) {
		deviceGui->setAccessType(deviceGui->getScsi(), deviceGui->getAccessType(deviceGui->getScsi()) + i);
	}
	else if(param.compare("root") == 0) {
        //auto raw = deviceGui->isRaw(deviceGui->getScsi());
        //auto max = raw ? mpc->getRawStoresAmount() : mpc->getStdStoresAmount();
		//if (deviceGui->getStore(deviceGui->getScsi()) == max - 1) {
			//return;
		//}
        deviceGui->setStore(deviceGui->getScsi(), deviceGui->getStore(deviceGui->getScsi()) + 1);
    }
}
