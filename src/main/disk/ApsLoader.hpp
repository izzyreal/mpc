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
    ApsLoader(mpc::Mpc&, std::shared_ptr<MpcFile>);
    static void loadFromParsedAps(mpc::file::aps::ApsParser&, mpc::Mpc&, bool withoutSounds, bool headless = false);

private:
    mpc::Mpc& mpc;
    std::weak_ptr<mpc::disk::MpcFile> file;

    void load();
    
    static void notFound(mpc::Mpc&, std::string soundFileName, std::string ext);
    static void static_load(void* this_p);
    static void loadSound(mpc::Mpc&, std::string soundFileName, std::string ext, std::weak_ptr<MpcFile> soundFile, bool replace, int loadSoundIndex, bool headless = false);
    static void showPopup(mpc::Mpc&, std::string name, std::string ext, int sampleSize);

};
}
