#include <disk/Store.hpp>

#include <disk/Stores.hpp>
#include <disk/AbstractDisk.hpp>

#include <Logger.hpp>

using namespace mpc::disk;
using namespace std;

Store::Store(Stores* stores, string path, int totalSpace, string volumeLabel, bool raw) 
{
	this->path = path;
	this->totalSpace = totalSpace;
	this->volumeLabel = volumeLabel;
	this->raw = raw;

	const string msg = "initializing store " + path + " with total space " + to_string(totalSpace);
	MLOG(msg);
}

string Store::toString()
{
	if (!raw)
	{
		return volumeLabel + " / " + AbstractDisk::formatFileSize(totalSpace);
	}

	return path + " / " + volumeLabel + " / " + AbstractDisk::formatFileSize(totalSpace);
}
