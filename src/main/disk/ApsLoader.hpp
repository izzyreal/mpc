#pragma once

#include <thread>
#include <memory>
#include <vector>
#include <string>

namespace mpc { class Mpc; }

namespace mpc::file::aps { class ApsParser; }

namespace mpc::sampler {
class Program;
}

namespace mpc::disk {
class MpcFile;
}

namespace mpc::disk {

class ApsLoader
{
    
private:
    mpc::Mpc& mpc;
    std::thread loadThread;
    std::weak_ptr<mpc::disk::MpcFile> file;
    
private:
    void load();
    static void notFound(mpc::Mpc&, std::string soundFileName, std::string ext);
    
private:
    static void static_load(void* this_p);
    
    static void loadSound(mpc::Mpc&, std::string soundFileName, std::string ext, std::weak_ptr<MpcFile> soundFile, bool replace, int loadSoundIndex, bool headless = false);
    
    static void showPopup(mpc::Mpc&, std::string name, std::string ext, int sampleSize);
    
public:
    ApsLoader(mpc::Mpc&, std::weak_ptr<MpcFile>);
    ~ApsLoader();
    
    static void loadFromParsedAps(mpc::file::aps::ApsParser& parsedAps, mpc::Mpc& mpc, bool headless = false);
    
};
}
