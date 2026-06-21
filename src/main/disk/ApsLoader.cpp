#include "ApsLoader.hpp"

#include "Mpc.hpp"
#include "disk/MpcFile.hpp"
#include "file/kaitai/ApsIo.hpp"

void mpc::disk::ApsLoader::load(mpc::Mpc &mpc,
                                const std::shared_ptr<mpc::disk::MpcFile> &file,
                                const bool headless)
{
    mpc::file::kaitai::ApsIo::load(mpc, file, headless);
}
