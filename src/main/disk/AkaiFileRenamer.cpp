#include "AkaiFileRenamer.hpp"

#include "FileIoPolicy.hpp"
#include "Mpc.hpp"
#include "Logger.hpp"

#include <algorithm>
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
    struct EntryToRename
    {
        mpc_fs::path path;
        bool isDirectory;
    };
    std::vector<EntryToRename> entriesToRename;
    const auto generateName = [](const mpc_fs::path &path, bool isDirectory,
                                 const std::vector<std::string> &usedNames)
    {
        const auto name = path.filename().string();
        if (isDirectory)
        {
            ShortNameGenerator generator(usedNames);
            return generator.generateShortName(
                ShortNameGenerator::tidyString(name)).asSimpleString();
        }
        return AkaiName::generate(name, usedNames);
    };

    auto dirItRes = mpc_fs::make_directory_iterator(p);
    if (!dirItRes)
    {
        throw std::runtime_error("Unable to normalize directory " + p.string());
    }

    for (auto e = *dirItRes; e != mpc_fs::directory_end(); ++e)
    {
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

        // Reserve every name that can stay as-is before allocating any new
        // names, including existing ~n suffixes. Otherwise iteration order can
        // let an imported file steal a name already referenced by a program.
        if (generateName(e->path(), *entryIsDirectoryRes, {}) ==
            e->path().filename().string())
        {
            existingNames.push_back(e->path().filename().string());
        }
        else
        {
            entriesToRename.push_back({e->path(), *entryIsDirectoryRes});
        }
    }

    // Keep suffix allocation deterministic for files that all need renaming.
    std::sort(entriesToRename.begin(), entriesToRename.end(),
              [](const auto &a, const auto &b)
              {
                  return a.path.filename().string() < b.path.filename().string();
              });
    for (const auto &entry : entriesToRename)
    {
        const auto akaiName =
            generateName(entry.path, entry.isDirectory, existingNames);
        existingNames.push_back(akaiName);
        if (!success(mpc_fs::rename(entry.path, tempRoot / akaiName),
                     FailurePolicy::Required,
                     "Akai rename move to temp root from '" +
                         entry.path.string() + "'"))
        {
            throw std::runtime_error("Unable to normalize directory " +
                                     p.string());
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
