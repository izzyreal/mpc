#pragma once
#include <disk/AbstractDisk.hpp>
#include <disk/Volume.hpp>
#include <disk/MountedVolumeSession.hpp>

#include <fat/AkaiFatLfnDirectory.hpp>
#include <fat/AkaiFatLfnDirectoryEntry.hpp>

#include <memory>
#include <vector>
#include <string>

namespace mpc::disk
{

    class MpcFile;

    class RawDisk : public AbstractDisk
    {
    public:
        using Mount = std::function<std::shared_ptr<MountedVolumeSession>(
            const Volume &)>;
        explicit RawDisk(Mpc &, Mount = {});
        ~RawDisk();

    private:
        Mount mount;
        std::shared_ptr<MountedVolumeSession> session;
        Volume volume;
        std::vector<std::shared_ptr<fat::AkaiFatLfnDirectoryEntry>> path;
        std::shared_ptr<fat::AkaiFatLfnDirectory> root;
        std::shared_ptr<fat::AkaiFatLfnDirectory> getDir();

    public:
        std::shared_ptr<MpcFile>
        newFile(const std::string &newFileName) override;
        void initFiles() override;
        std::unique_ptr<SaveDestination>
        captureSaveDestination(int view) override;
        std::string getDirectoryName() override;
        bool moveBack() override;
        bool moveForward(const std::string &directoryName) override;
        void close() override;
        void flush() override;
        bool deleteAllFiles(int dwGuiDelete) override;
        bool newFolder(const std::string &newDirName) override;
        std::string getAbsolutePath() override;
        std::string getTypeShortName() override;

        uint64_t getTotalSize() override;
        std::string getVolumeLabel() override;
        Volume &getVolume() override
        {
            return volume;
        }
        void initRoot() override
        {
            if (isSaveBusy())
            {
                return;
            }
            if (root)
            {
                return;
            }
            session = mount(volume);
            root = session->getRoot();
            if (!root)
            {
                throw std::runtime_error("Unable to mount device");
            }
        }

    protected:
        int getPathDepth() override;
    };
} // namespace mpc::disk
