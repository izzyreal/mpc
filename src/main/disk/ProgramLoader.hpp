#pragma once

#include <memory>
#include <vector>
#include <string>

namespace mpc { class Mpc; }

namespace mpc::sampler
{
class Program;
}

namespace mpc::disk
{
class MpcFile;
}

namespace mpc::disk
{
class ProgramLoader
{
public:
    static std::shared_ptr<mpc::sampler::Program> loadProgram(mpc::Mpc&, std::shared_ptr<mpc::disk::MpcFile>, const int replaceIndex);
    
private:
    static void loadSound(mpc::Mpc &, const std::string &soundName, const std::string &ext,
                          std::shared_ptr<MpcFile> soundFile, std::vector<int> *soundsDestIndex, const bool replace,
                          const int loadSoundIndex);
    static void showPopup(mpc::Mpc&, std::string name, std::string ext, int sampleSize);
    static void notFound(mpc::Mpc &, std::string soundFileName);

};
}
