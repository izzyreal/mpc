#pragma once

#include <memory>
#include <vector>

namespace mpc { class Mpc; }
namespace mpc::disk { class AbstractDisk; }
namespace mpc::lcdgui::screens { class LoadScreen; }

namespace mpc::disk
{
	class DiskController
	{

	private:
		mpc::Mpc& mpc;
        std::vector<std::shared_ptr<AbstractDisk>> disks;
        int activeDiskIndex = 0;

        void initDisks();

        friend class mpc::lcdgui::screens::LoadScreen;
        
	public:
        std::vector<std::shared_ptr<AbstractDisk>>& getDisks();
		DiskController(mpc::Mpc&);
		std::weak_ptr<AbstractDisk> getActiveDisk();

	};
}
