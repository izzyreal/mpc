#include "DiskController.hpp"

#include <Mpc.hpp>

#include <disk/AbstractDisk.hpp>
#include <disk/StdDisk.hpp>
#include <disk/RawDisk.hpp>
#include <disk/Store.hpp>
#include <disk/Stores.hpp>

#include <ui/Uis.hpp>
#include <ui/vmpc/DeviceGui.hpp>

using namespace mpc;
using namespace std;

DiskController::DiskController()
{
	
	disks = vector<shared_ptr<mpc::disk::AbstractDisk>>(MAX_DISKS);
	stores = make_shared<mpc::disk::Stores>();
}

const int DiskController::MAX_DISKS;

void DiskController::initDisks()
{
	disks.clear();
	disks = vector<shared_ptr<mpc::disk::AbstractDisk>>(MAX_DISKS);
	auto deviceGui = Mpc::instance().getUis().lock()->getDeviceGui();
	for (int i = 0; i < MAX_DISKS; i++) {
		auto oldDisk = disks[i];
		if (!deviceGui->isEnabled(i) && oldDisk) {
			oldDisk->close();
			disks[i].reset();
			continue;
		}
		if (deviceGui->getStore(i) == -1)
			continue;

		weak_ptr<mpc::disk::Store> candidate;
		candidate = deviceGui->isRaw(i) && stores->getRawStores().size() > deviceGui->getStore(i) ? stores->getRawStore(deviceGui->getStore(i)) : stores->getStdStore(deviceGui->getStore(i));
		if (oldDisk) {
			auto oldStore = oldDisk->getStore().lock();
			if (oldStore == candidate.lock())
				continue;

			oldDisk->close();
		}
		if (deviceGui->isRaw(i) && stores->getRawStores().size() > deviceGui->getStore(i)) {
			try {
				disks[i] = make_shared<mpc::disk::RawDisk>(stores->getRawStore(deviceGui->getStore(i)));
			}
			catch (exception e) {
				deviceGui->setAccessType(i, mpc::ui::vmpc::DeviceGui::STD);
				deviceGui->setStore(i, 0);
				deviceGui->saveSettings();
				disks[i] = make_shared<mpc::disk::StdDisk>(stores->getStdStore(deviceGui->getStore(i)));
			}
		}
		else {
			deviceGui->setAccessType(i, mpc::ui::vmpc::DeviceGui::STD);
			deviceGui->setStore(i, 0);
			deviceGui->saveSettings();
			disks[i] = make_shared<mpc::disk::StdDisk>(stores->getStdStore(deviceGui->getStore(i)));
		}
	}
}

weak_ptr<mpc::disk::Stores> DiskController::getStores()
{
    return stores;
}

weak_ptr<mpc::disk::AbstractDisk> DiskController::getDisk()
{
	if (disks.size() == 0) return weak_ptr<mpc::disk::AbstractDisk>();
	return disks[Mpc::instance().getUis().lock()->getDeviceGui()->getScsi()];
}

weak_ptr<mpc::disk::AbstractDisk> DiskController::getDisk(int i)
{
	return disks[i];
}

DiskController::~DiskController() {
}
