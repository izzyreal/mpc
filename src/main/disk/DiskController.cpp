#include "DiskController.hpp"

#include <Mpc.hpp>

#include <disk/AbstractDisk.hpp>
#include <disk/StdDisk.hpp>
#include <disk/Store.hpp>
#include <disk/Stores.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/VmpcDiskScreen.hpp>

using namespace mpc::disk;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace std;

const int DiskController::MAX_DISKS;

DiskController::DiskController(mpc::Mpc& mpc)
	: mpc(mpc)
{
	stores = make_shared<Stores>();
}

void DiskController::initDisks()
{
	disks.clear();
	disks = vector<shared_ptr<mpc::disk::AbstractDisk>>(MAX_DISKS);
	auto vmpcDiskScreen = mpc.screens->get<VmpcDiskScreen>("vmpc-disk");

	for (int i = 0; i < MAX_DISKS; i++)
	{
		auto oldDisk = disks[i];
	
		if (!vmpcDiskScreen->isEnabled(i) && oldDisk)
		{
			oldDisk->close();
			disks[i].reset();
			continue;
		}
		
		if (vmpcDiskScreen->getStore(i) == -1)
		{
			continue;
		}

		weak_ptr<mpc::disk::Store> candidate;
		candidate = vmpcDiskScreen->isRaw(i) && stores->getRawStores().size() > vmpcDiskScreen->getStore(i) ? stores->getRawStore(vmpcDiskScreen->getStore(i)) : stores->getStdStore(vmpcDiskScreen->getStore(i));
	
		if (oldDisk)
		{
			auto oldStore = oldDisk->getStore().lock();

			if (oldStore == candidate.lock())
			{
				continue;
			}

			oldDisk->close();
		}

		vmpcDiskScreen->setAccessType(i, VmpcDiskScreen::STD);
		vmpcDiskScreen->setStore(i, 0);
		vmpcDiskScreen->saveSettings();
		
		disks[i] = make_shared<mpc::disk::StdDisk>(mpc, stores->getStdStore(vmpcDiskScreen->getStore(i)));
	}
}

weak_ptr<mpc::disk::Stores> DiskController::getStores()
{
    return stores;
}

weak_ptr<mpc::disk::AbstractDisk> DiskController::getDisk()
{
	if (disks.size() == 0)
	{
		return {};
	}

	auto vmpcDiskScreen = mpc.screens->get<VmpcDiskScreen>("vmpc-disk");
	return disks[vmpcDiskScreen->scsi];
}

weak_ptr<mpc::disk::AbstractDisk> DiskController::getDisk(int i)
{
	return disks[i];
}
