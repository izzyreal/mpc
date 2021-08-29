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
private:
    mpc::Mpc& mpc;
    std::shared_ptr<mpc::sampler::Program> result;
    
    std::shared_ptr<MpcFile> file;
    bool replace = false;
    
private:
    void loadProgram(const int replaceIndex);
    static void static_loadProgram(void* this_p, const int replaceIndex);
    
    void loadSound(const std::string& soundFileName, const std::string& soundName, const std::string& ext, std::shared_ptr<MpcFile> soundFile, std::vector<int>* soundsDestIndex, const bool replace, const int loadSoundIndex);
    void showPopup(std::string name, std::string ext, int sampleSize);
    void notFound(std::string soundFileName, std::string ext);
    
public:
    std::shared_ptr<mpc::sampler::Program> get();
    
    ProgramLoader(mpc::Mpc&, std::shared_ptr<MpcFile>, const int replaceIndex);
};
}
