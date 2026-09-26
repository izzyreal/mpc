#pragma once

#include "disk/MpcFile.hpp"
#include <functional>

namespace akaifat::fat
{
    class AkaiFatLfnDirectory;
}

namespace mpc::disk
{
    struct DirectoryListing
    {
        std::vector<std::shared_ptr<MpcFile>> files, allFiles, parentFiles;
    };

    // All handles and policies are captured before leaving the UI thread.
    // The operation exclusively owns backend access until its result is
    // applied.
    class SaveDestination
    {
        std::function<std::vector<std::shared_ptr<MpcFile>>()> listCurrent,
            listParent;
        std::function<std::shared_ptr<MpcFile>(const std::string &)> createFile;
        bool sorted, writable;
        int view;
        std::function<void()> normalize;
        std::function<void()> flushVolume;
        std::vector<std::shared_ptr<MpcFile>> entries;

    public:
        static std::unique_ptr<SaveDestination>
        fromRaw(std::shared_ptr<akaifat::fat::AkaiFatLfnDirectory> directory,
                std::shared_ptr<akaifat::fat::AkaiFatLfnDirectory> parent,
                bool writable, int view, std::function<void()> flush);
        SaveDestination(
            std::function<std::vector<std::shared_ptr<MpcFile>>()> listCurrent,
            std::function<std::vector<std::shared_ptr<MpcFile>>()> listParent,
            std::function<std::shared_ptr<MpcFile>(const std::string &)>
                createFile,
            bool sorted, bool writable, int view,
            std::function<void()> normalize, std::function<void()> flushVolume);
        virtual ~SaveDestination() = default;
        virtual void prepare();
        std::shared_ptr<MpcFile> find(const std::string &name) const;
        virtual std::shared_ptr<MpcFile> create(const std::string &name,
                                                bool replace);
        void remove(const std::shared_ptr<MpcFile> &file);
        virtual void flush();
        virtual DirectoryListing scan();
    };
} // namespace mpc::disk
