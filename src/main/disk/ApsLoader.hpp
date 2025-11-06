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
        static void load(mpc::Mpc &,
                         const std::shared_ptr<mpc::disk::MpcFile> &);
        static void loadFromParsedAps(mpc::file::aps::ApsParser &, mpc::Mpc &,
                                      bool withoutSounds,
                                      bool headless = false);

    private:
        static void handleSoundNotFound(mpc::Mpc &,
                                        const std::string &soundFileName);
        static void loadSound(mpc::Mpc &, const std::string &soundFileName,
                              const std::string &ext,
                              const std::weak_ptr<MpcFile> &soundFile,
                              bool headless = false);
        static void showPopup(mpc::Mpc &, const std::string &name,
                              const std::string &ext, int sampleSize);
    };
} // namespace mpc::disk
