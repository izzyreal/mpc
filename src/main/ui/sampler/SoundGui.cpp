#include "SoundGui.hpp"

#include <cmath>

using namespace mpc::ui::sampler;
using namespace std;

void SoundGui::setPlayX(int i)
{
	if (i < 0 || i > 4)
	{
		return;
	}
    playX = i;
    setChanged();
    notifyObservers(string("playx"));
}

int SoundGui::getPlayX()
{
    return playX;
}

void SoundGui::setView(int i)
{
	if (i < 0 || i > 1)
	{
		return;
	}
	view = i;
	setChanged();
	notifyObservers(string("view"));
}

int SoundGui::getView()
{
    return view;
}

void SoundGui::initZones(int length)
{
	if (zone > numberOfZones - 1) zone = 0;
	this->totalLength = length;
	int zoneLength = (int)floor(length  / numberOfZones);
	int zoneStart = 0;
	zones.clear();
	for (int i = 0; i < numberOfZones - 1; i++) {
		zones.push_back(vector<int>(2));
		zones[i][0] = zoneStart;
		zones[i][1] = zoneStart + zoneLength - 1;
		zoneStart += zoneLength;
	}
	zones.push_back(vector<int>(2));
	zones[numberOfZones - 1][0] = zoneStart;
	zones[numberOfZones - 1][1] = length;
}

void SoundGui::setZoneStart(int zoneIndex, int start)
{
	if (start > zones[zoneIndex][1]) {
		start = zones[zoneIndex][1];
	}
	if (zoneIndex == 0 && start < 0) {
		start = 0;
	}
	if (zoneIndex > 0 && start < zones[zoneIndex - 1][0]) {
		start = zones[zoneIndex - 1][0];
	}
	zones[zoneIndex][0] = start;
	if (zoneIndex != 0) {
		zones[zoneIndex - 1][1] = start;
	}
	setChanged();
	notifyObservers(string("zone"));
}

int SoundGui::getZoneStart(int zoneIndex)
{
    return zones[zoneIndex][0];
}

void SoundGui::setZoneEnd(int zoneIndex, int end)
{
	if (end < zones[zoneIndex][0]) {
		end = zones[zoneIndex][0];
	}
	if (zoneIndex < numberOfZones - 1 && end > zones[zoneIndex + 1][1]) {
		end = zones[zoneIndex + 1][1];
	}
	if (zoneIndex == numberOfZones - 1 && end > totalLength) {
		end = totalLength;
	}
	zones[zoneIndex][1] = end;
	if (zoneIndex != numberOfZones - 1) {
		zones[zoneIndex + 1][0] = end;
	}
	setChanged();
	notifyObservers(string("zone"));
}

int SoundGui::getZoneEnd(int zoneIndex)
{
	return zones[zoneIndex][1];
}

void SoundGui::setZone(int i)
{
	if (i < 0 || i > numberOfZones - 1) return;
	zone = i;
	setChanged();
	notifyObservers(string("zone"));
}

int SoundGui::getZoneNumber()
{
    return zone;
}

void SoundGui::setNumberOfZones(int i)
{
	if (i < 1 || i > 16) return;
	numberOfZones = i;
	setChanged();
	notifyObservers(string("numberofzones"));
}

int SoundGui::getNumberOfZones()
{
    return numberOfZones;
}

void SoundGui::setPreviousNumberOfZones(int i)
{
    previousNumberOfZones = i;
}

int SoundGui::getPreviousNumberOfzones()
{
    return previousNumberOfZones;
}