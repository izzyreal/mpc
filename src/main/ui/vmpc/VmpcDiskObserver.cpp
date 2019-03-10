#include <ui/vmpc/VmpcDiskObserver.hpp>

#include <Mpc.hpp>
#include <disk/Store.hpp>
#include <disk/Stores.hpp>
#include <ui/Uis.hpp>
#include <lcdgui/Field.hpp>
#include <ui/vmpc/DeviceGui.hpp>

using namespace mpc::ui::vmpc;
using namespace std;

VmpcDiskObserver::VmpcDiskObserver(mpc::Mpc* mpc) 
{
	this->mpc = mpc;
	deviceGui = mpc->getUis().lock()->getDeviceGui();
	deviceGui->deleteObservers();
	deviceGui->addObserver(this);
	auto ls = mpc->getLayeredScreen().lock();
	scsiField = ls ->lookupField("scsi");
	accessTypeField = ls->lookupField("accesstype");
	rootField = ls->lookupField("root");
	rootField.lock()->setScrolling(true);
	displayScsi();
	displayAccessType();
	displayRoot();
}

void VmpcDiskObserver::update(moduru::observer::Observable* o, std::any a)
{
	displayScsi();
	displayAccessType();
	displayRoot();
}

void VmpcDiskObserver::displayScsi()
{
	scsiField.lock()->setText(to_string(deviceGui->getScsi() + 1));
}

void VmpcDiskObserver::displayAccessType()
{
    vector<string> types = vector<string>{ "OFF", "STD", "RAW" };
    accessTypeField.lock()->setText(types[deviceGui->getAccessType(deviceGui->getScsi())]);
}

void VmpcDiskObserver::displayRoot()
{
	string root = "< SCSI-" + to_string(deviceGui->getScsi() + 1) + " disconnected >";
	int store = deviceGui->getStore(deviceGui->getScsi());
	auto stores = mpc->getStores().lock();
	if (store != -1) {
		if (deviceGui->isRaw(deviceGui->getScsi())) {
			root = stores->getRawStore(store).lock()->volumeLabel;
		}
		else {
			root = stores->getStdStore(store).lock()->path;
		}
	}
	rootField.lock()->setText(root);
}

VmpcDiskObserver::~VmpcDiskObserver() {
	rootField.lock()->setScrolling(false);
}
