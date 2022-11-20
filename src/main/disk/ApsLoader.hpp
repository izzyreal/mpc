#pragma once

#include <memory>
#include <vector>
#include <string>

namespace mpc { class Mpc; }
namespace mpc::sampler { class Program; }
namespace mpc::disk { class MpcFile; }
namespace mpc::file::aps { class ApsParser; }

namespace mpc::disk {
class ApsLoader
{
public:
    static void load(mpc::Mpc&, std::shared_ptr<mpc::disk::MpcFile>);
    static void loadFromParsedAps(mpc::file::aps::ApsParser&, mpc::Mpc&, bool withoutSounds, bool headless = false);

private:
    static void handleSoundNotFound(mpc::Mpc &, std::string soundFileName);
    static void loadSound(mpc::Mpc&, std::string soundFileName, std::string ext, std::weak_ptr<MpcFile> soundFile, bool headless = false);
    static void showPopup(mpc::Mpc&, std::string name, std::string ext, int sampleSize);

};
}
