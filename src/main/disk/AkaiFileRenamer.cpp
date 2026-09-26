#include "AkaiFileRenamer.hpp"

#include "FileIoPolicy.hpp"
#include "Mpc.hpp"
#include "Logger.hpp"

#include <stdexcept>

#include "ShortNameGenerator.hpp"
#include "ShortName.hpp"
#include "file/AkaiName.hpp"

using namespace mpc::disk;
using namespace mpc::file_io;
using namespace mpc::file;

void AkaiFileRenamer::renameFilesInDirectory(Mpc &mpc, const mpc_fs::path &p)
{
    try
    {
        renameFilesInDirectory(p, mpc.paths->getDocuments()->tempPath());
    }
    catch (const std::exception &e)
    {
        MLOG(e.what());
    }
}

void AkaiFileRenamer::renameFilesInDirectory(const mpc_fs::path &p,
                                             const mpc_fs::path &tempRoot)
{
    const auto isDirectoryRes = mpc_fs::is_directory(p);
    if (!isDirectoryRes)
    {
        MLOG("AkaiFileRenamer: failed to inspect '" + p.string() + "': " +
             isDirectoryRes.error().message);
        throw std::runtime_error("Unable to normalize directory " + p.string());
    }
    if (!*isDirectoryRes)
    {
        MLOG("AkaiFileRenamer: path is not a directory: '" + p.string() + "'");
        throw std::runtime_error("Unable to normalize directory " + p.string());
    }


    const auto tempRootExistsValue = value(
        mpc_fs::exists(tempRoot), FailurePolicy::Required,
        "Akai rename temp root existence check for '" + tempRoot.string() + "'");
    if (!tempRootExistsValue)
    {
        throw std::runtime_error("Unable to normalize directory " + p.string());
    }

    if (*tempRootExistsValue)
    {
        const auto removeAllValue = value(
            mpc_fs::remove_all(tempRoot), FailurePolicy::Required,
            "Akai rename temp root cleanup for '" + tempRoot.string() + "'");
        if (!removeAllValue)
        {
            throw std::runtime_error("Unable to normalize directory " +
                                     p.string());
        }
    }

    auto tempRootCreateRes = mpc_fs::create_directories(tempRoot);
    if (!tempRootCreateRes)
    {
        logFailure(FailurePolicy::Required,
                   "Akai rename temp root create for '" + tempRoot.string() +
                       "'",
                   tempRootCreateRes.error());
        throw std::runtime_error("Unable to normalize directory " + p.string());
    }

    std::vector<std::string> existingNames;

    auto dirItRes = mpc_fs::make_directory_iterator(p);
    if (!dirItRes)
    {
        throw std::runtime_error("Unable to normalize directory " + p.string());
    }

    for (auto e = *dirItRes; e != mpc_fs::directory_end(); ++e)
    {
        std::string akaiName;

        if (e->path().filename() == ".DS_Store")
        {
            continue;
        }

        const auto entryIsDirectoryRes = mpc_fs::is_directory(*e);
        if (!entryIsDirectoryRes)
        {
            MLOG("AkaiFileRenamer: failed to inspect entry '" +
                 e->path().string() + "': " + entryIsDirectoryRes.error().message);
            throw std::runtime_error("Unable to normalize directory " +
                                     p.string());
        }

        if (*entryIsDirectoryRes)
        {
            const auto tidyString =
                ShortNameGenerator::tidyString(e->path().filename().string());
            ShortNameGenerator generator(existingNames);
            akaiName = generator.generateShortName(tidyString).asSimpleString();
        }
        else
        {
            akaiName =
                AkaiName::generate(e->path().filename().string(), existingNames);
        }

        if (akaiName != e->path().filename().string())
        {
            existingNames.push_back(akaiName);
            if (!success(mpc_fs::rename(e->path(), tempRoot / akaiName),
                         FailurePolicy::Required,
                         "Akai rename move to temp root from '" +
                             e->path().string() + "'"))
            {
                throw std::runtime_error("Unable to normalize directory " +
                                         p.string());
            }
        }
        else
        {
            existingNames.push_back(e->path().filename().string());
        }
    }

    auto tempDirItRes = mpc_fs::make_directory_iterator(tempRoot);
    if (!tempDirItRes)
    {
        throw std::runtime_error("Unable to normalize directory " + p.string());
    }

    for (auto e = *tempDirItRes; e != mpc_fs::directory_end(); ++e)
    {
        if (!success(mpc_fs::rename(e->path(), p / e->path().filename()),
                     FailurePolicy::Required,
                     "Akai rename restore from temp root for '" +
                         e->path().string() + "'"))
        {
            throw std::runtime_error("Unable to normalize directory " +
                                     p.string());
        }
    }
}
