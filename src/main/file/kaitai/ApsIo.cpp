#include "file/kaitai/ApsIo.hpp"

#include "Mpc.hpp"
#include "disk/ApsLoader.hpp"
#include "disk/MpcFile.hpp"
#include "file/aps/ApsParser.hpp"
#include "file/kaitai/KaitaiIoUtil.hpp"
#include "file/kaitai/generated/mpc2000xl_aps.h"
#include "sampler/Sampler.hpp"

using namespace mpc::file::kaitai;

void ApsIo::load(mpc::Mpc &mpc, const std::shared_ptr<mpc::disk::MpcFile> &file, bool headless)
{
    const auto canonicalBytes = parseRewrite<mpc2000xl_aps_t>(file->getBytes());
    mpc::file::aps::ApsParser parser(canonicalBytes);

    if (!parser.isHeaderValid())
    {
        throw std::runtime_error("Invalid APS header");
    }

    constexpr auto withoutSounds = false;
    mpc::disk::ApsLoader::loadFromParsedAps(parser, mpc, withoutSounds, headless);
    mpc.getSampler()->setSoundIndex(0);
}

std::vector<char> ApsIo::save(mpc::Mpc &mpc, const std::string &apsName)
{
    mpc::file::aps::ApsParser parser(mpc, apsName);
    return parseRewrite<mpc2000xl_aps_t>(parser.getBytes());
}
