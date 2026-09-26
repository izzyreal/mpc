#pragma once

#include <disk/Volume.hpp>
#include <functional>
#include <fstream>
#include <memory>

namespace akaifat
{
    class ImageBlockDevice;
    namespace fat
    {
        class AkaiFatFileSystem;
        class AkaiFatLfnDirectory;
    } // namespace fat
} // namespace akaifat

namespace mpc::disk
{
    // Owns an open filesystem and its platform access independently of the
    // persistent binding and browser navigation. Never uses Mpc on destruction.
    class MountedVolumeSession
    {
    public:
        using Open = std::function<std::fstream(const std::string &, bool)>;
        using Release = std::function<void(const std::string &)>;
        explicit MountedVolumeSession(const Volume &, Open = {}, Release = {});
        ~MountedVolumeSession();
        MountedVolumeSession(const MountedVolumeSession &) = delete;
        MountedVolumeSession &operator=(const MountedVolumeSession &) = delete;
        std::shared_ptr<akaifat::fat::AkaiFatLfnDirectory> getRoot() const;
        bool isReadOnly() const;
        void flush();
        void close();

    private:
        void releaseResources();
        std::string source;
        Release release;
        bool accessAcquired = false;
        std::fstream stream;
        std::shared_ptr<akaifat::ImageBlockDevice> device;
        std::unique_ptr<akaifat::fat::AkaiFatFileSystem> filesystem;
    };
} // namespace mpc::disk
