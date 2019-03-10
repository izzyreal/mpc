#include <ui/vmpc/DeviceGui.hpp>

using namespace mpc::ui::vmpc;
using namespace std;

DeviceGui::DeviceGui()
{
	accessTypes = vector<int>(7);
	stores = vector<int>(7);
	accessTypesPlaceHolder = vector<int>(7);
	storesPlaceHolder = vector<int>(7);

	for (int i = 0; i < 7; i++)
		stores[i] = -1;

	stores[0] = 0;
	accessTypes[0] = RAW;
	saveSettings();
	initialized = true;
}

const int mpc::ui::vmpc::DeviceGui::OFF;
const int mpc::ui::vmpc::DeviceGui::STD;
const int mpc::ui::vmpc::DeviceGui::RAW;

void DeviceGui::setScsi(int i)
{
	if (i < 0 || i > 6) return;
	scsi = i;
	setChanged();
	notifyObservers(string("scsi"));
}

void DeviceGui::setStore(int i, int store)
{
	if (accessTypes[i] == STD && store > 0) return;
	if (store < -1) return;

	stores[i] = store;
	setChanged();
	notifyObservers(string("root"));
}

int DeviceGui::getStore(int i)
{
    return stores[i];
}

int DeviceGui::getScsi()
{
    return scsi;
}

void DeviceGui::notifyAccessType()
{
    setChanged();
	notifyObservers(string("accesstype"));
}

bool DeviceGui::isEnabled(int i)
{
    return stores[i] != -1 && accessTypes[i] != OFF;
}

bool DeviceGui::isRaw(int i)
{
    return accessTypes[i] == RAW;
}

int DeviceGui::getAccessType(int i)
{
    return accessTypes[i];
}

void DeviceGui::setAccessType(int i, int type)
{
	if (type < OFF || type > RAW) return;
	if (accessTypes[i] == type) return;

	accessTypes[i] = type;
	stores[i] = -1;
	notifyAccessType();
}

void DeviceGui::saveSettings()
{
	scsiPlaceHolder = scsi;
	for (int i = 0; i < 7; i++) {
		accessTypesPlaceHolder[i] = accessTypes[i];
		storesPlaceHolder[i] = stores[i];
	}
}

void DeviceGui::restoreSettings()
{
	if (!initialized) return;
	scsi = scsiPlaceHolder;
	for (int i = 0; i < 7; i++) {
		accessTypes[i] = accessTypesPlaceHolder[i];
		stores[i] = storesPlaceHolder[i];
	}
}
