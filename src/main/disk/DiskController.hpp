#pragma once

#include <memory>
#include <vector>

namespace mpc { class Mpc; }
namespace mpc::disk { class AbstractDisk; }

namespace mpc::disk
{
	class DiskController
	{

	private:
		mpc::Mpc& mpc;
        std::vector<std::shared_ptr<AbstractDisk>> disks;
		
	public:
        std::vector<std::shared_ptr<AbstractDisk>>& getDisks();
		DiskController(mpc::Mpc&);
		void initDisks();
		std::weak_ptr<AbstractDisk> getActiveDisk();

	};
}
