#pragma once

#include <memory>
#include <string>

namespace mpc
{
    class Mpc;
}
namespace mpc::sampler
{
    class Program;
}
namespace mpc::disk
{
    class MpcFile;
}
namespace mpc::file::aps
{
    class ApsParser;
}

namespace mpc::disk
{
    class ApsLoader
    {
    public:
        static void load(Mpc &, const std::shared_ptr<MpcFile> &);
        static void loadFromParsedAps(file::aps::ApsParser &, Mpc &,
                                      bool withoutSounds,
                                      bool headless = false);

    private:
        static void handleSoundNotFound(Mpc &,
                                        const std::string &soundFileName);
        static void loadSound(Mpc &, const std::string &soundFileName,
                              const std::string &ext,
                              const std::weak_ptr<MpcFile> &soundFile,
                              bool headless = false);
        static void showPopup(Mpc &, const std::string &name,
                              const std::string &ext, int sampleSize);
    };
} // namespace mpc::disk
