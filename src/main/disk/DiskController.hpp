#pragma once

#include <memory>
#include <vector>

namespace mpc::disk
{
	class Stores;
	class AbstractDisk;
}

namespace mpc::disk
{
	class DiskController
	{


	private:
		static const int MAX_DISKS = 7;
		std::vector<std::shared_ptr<AbstractDisk>> disks = std::vector<std::shared_ptr<AbstractDisk>>();
		
		/*
		* A disk potentially has multiple stores/volumes/partitions. Each store can be mapped to a single
		* ATAPI or SCSI device (an AbstractDisk) of the virtual MPC2000XL.
		*/
		std::shared_ptr<Stores> stores;

	public:
		DiskController();

		void initDisks();
		std::weak_ptr<Stores> getStores();
		std::weak_ptr<AbstractDisk> getDisk();
		std::weak_ptr<AbstractDisk> getDisk(int i);

	};
}
