#pragma once

#include <memory>
#include <vector>

namespace mpc
{
    class Mpc;
}
namespace mpc::disk
{
    class AbstractDisk;
}

namespace mpc::disk
{
    class DiskController
    {
        Mpc &mpc;
        std::vector<std::shared_ptr<AbstractDisk>> disks;
        int activeDiskIndex = 0;

        void initDisks();

    public:
        explicit DiskController(Mpc &);
        std::vector<std::shared_ptr<AbstractDisk>> &getDisks();
        std::shared_ptr<AbstractDisk> getActiveDisk();
        int getActiveDiskIndex() const;
        void setActiveDiskIndex(int newActiveDiskIndex);

        void detectRawUsbVolumes();
    };
} // namespace mpc::disk
