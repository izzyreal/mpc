#include "VmpcDiskScreen.hpp"

#include <disk/Store.hpp>
#include <disk/Stores.hpp>

using namespace mpc::lcdgui::screens;
using namespace std;

VmpcDiskScreen::VmpcDiskScreen(const int layerIndex)
	: ScreenComponent("vmpc-disk", layerIndex)
{
}

void VmpcDiskScreen::open()
{
	displayScsi();
	displayAccessType();
	displayRoot();
}

void VmpcDiskScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 0:
		ls.lock()->openScreen("vmpc-settings");
		break;
	}
}

void VmpcDiskScreen::turnWheel(int i)
{
    init();

	if (param.compare("scsi") == 0)
	{
		setScsi(scsi + i);
	}
	else if (param.compare("accesstype") == 0)
	{
		setAccessType(scsi, getAccessType(scsi) + i);
	}
	else if(param.compare("root") == 0)
	{
        setStore(scsi, getStore(scsi) + 1);
    }
}

const int VmpcDiskScreen::OFF;
const int VmpcDiskScreen::STD;
const int VmpcDiskScreen::RAW;

void VmpcDiskScreen::setScsi(int i)
{
	if (i < 0 || i > 6)
	{
		return;
	}
	
	scsi = i;
	displayScsi();
}

void VmpcDiskScreen::setStore(int i, int store)
{
	if (accessTypes[i] == STD && store > 0)
	{
		return;
	}
	
	if (store < -1)
	{
		return;
	}

	stores[i] = store;
	displayRoot();
}

int VmpcDiskScreen::getStore(int i)
{
	return stores[i];
}

bool VmpcDiskScreen::isEnabled(int i)
{
	return stores[i] != -1 && accessTypes[i] != OFF;
}

bool VmpcDiskScreen::isRaw(int i)
{
	return accessTypes[i] == RAW;
}

int VmpcDiskScreen::getAccessType(int i)
{
	return accessTypes[i];
}

void VmpcDiskScreen::setAccessType(int i, int type)
{
	if (type < OFF || type > RAW)
	{
		return;
	}

	if (accessTypes[i] == type)
	{
		return;
	}

	accessTypes[i] = type;
	stores[i] = -1;
	displayAccessType();
	displayRoot();
}

void VmpcDiskScreen::saveSettings()
{
	scsiPlaceHolder = scsi;

	for (int i = 0; i < 7; i++)
	{
		accessTypesPlaceHolder[i] = accessTypes[i];
		storesPlaceHolder[i] = stores[i];
	}
}

void VmpcDiskScreen::restoreSettings()
{
	scsi = scsiPlaceHolder;
	
	for (int i = 0; i < 7; i++)
	{
		accessTypes[i] = accessTypesPlaceHolder[i];
		stores[i] = storesPlaceHolder[i];
	}
}

void VmpcDiskScreen::displayScsi()
{
	findField("scsi").lock()->setText(to_string(scsi + 1));
}

void VmpcDiskScreen::displayAccessType()
{
	findField("access-type").lock()->setText(types[getAccessType(scsi)]);
}

void VmpcDiskScreen::displayRoot()
{
	string root = "< SCSI-" + to_string(scsi + 1) + " disconnected >";

	int store = getStore(scsi);
	auto stores = Mpc::instance().getStores().lock();

	if (store != -1)
	{
		if (isRaw(scsi))
		{
			root = stores->getRawStore(store).lock()->volumeLabel;
		}
		else {
			root = stores->getStdStore(store).lock()->path;
		}
	}

	findField("root").lock()->setText(root);
}
