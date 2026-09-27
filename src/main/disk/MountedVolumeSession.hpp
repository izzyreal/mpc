#pragma once

#include <disk/Volume.hpp>
#include <functional>
#include <fstream>
#include <memory>

namespace akaifat
{
    class BlockDevice;
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
        using ImageOpen = std::function<std::shared_ptr<akaifat::BlockDevice>(
            const Volume &, bool)>;
        static std::shared_ptr<MountedVolumeSession> openImage(const Volume &,
                                                               ImageOpen = {});
        explicit MountedVolumeSession(const Volume &, Open = {}, Release = {});
        ~MountedVolumeSession();
        MountedVolumeSession(const MountedVolumeSession &) = delete;
        MountedVolumeSession &operator=(const MountedVolumeSession &) = delete;
        std::shared_ptr<akaifat::fat::AkaiFatLfnDirectory> getRoot() const;
        bool isReadOnly() const;
        uint64_t getSize() const;
        void flush();
        void close();

    private:
        MountedVolumeSession(std::shared_ptr<akaifat::BlockDevice>, bool);
        void releaseResources();
        std::string source;
        Release release;
        bool accessAcquired = false;
        std::fstream stream;
        std::shared_ptr<akaifat::BlockDevice> device;
        std::unique_ptr<akaifat::fat::AkaiFatFileSystem> filesystem;
    };
} // namespace mpc::disk
