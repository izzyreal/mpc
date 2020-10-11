#pragma once

#include <string>

/*
* A Store either points, via its path, to a volume (f.e. "D:\") that is currently unmounted, or to a directory
* on the user's drive that contains SND, WAV, APS, ALL, etc. files that the user wants to access inside vMPC2000XL.
* If it points to an unmounted volume, it is a raw volume, meaning it should be accessed via raw disk volume access,
* interpreting its partition as an Akai FAT file system with 16.3 file names.
*
* The Store indirectly represents a potential disk device for the vMPC2000XL. Consider it like a hardware device, that the user
* can choose to connect or not.
* 
* This connecting and disconnecting, and the mapping of a Store to a StdDevice or to a RawDevice, happens in the LCD GUI through the vMPC2000XL-only screen "Device".
* Only when the DeviceScreen has the index of a Store (as managed by the Stores class) inside its std::vector<int> stores, will DiskController attempt to instantiate
* a StdDisk or other implementation of AbstractDisk (for example RawDisk).
*/

namespace mpc {
	namespace disk {

		class Stores;

		class Store final
		{

		public:
			bool raw { false };
			std::string path  = "";
			int totalSpace { 0 };
			std::string volumeLabel  = "";

		public:
			std::string toString();
			Store(Stores* stores, std::string path, int totalSpace, std::string volumeLabel, bool raw);

		public:
			Stores* stores{ nullptr };

		};

	}
}
