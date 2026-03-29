#include "AkaiFileRenamer.hpp"

#include "FileIoPolicy.hpp"
#include "Mpc.hpp"
#include "Logger.hpp"

#include <cassert>

#include "ShortNameGenerator.hpp"
#include "ShortName.hpp"
#include "file/AkaiName.hpp"

using namespace mpc::disk;
using namespace mpc::file_io;
using namespace mpc::file;

void AkaiFileRenamer::renameFilesInDirectory(Mpc &mpc, const mpc_fs::path &p)
{
    const auto isDirectoryRes = mpc_fs::is_directory(p);
    assert(isDirectoryRes.value_or(false));
    if (!isDirectoryRes)
    {
        MLOG("AkaiFileRenamer: failed to inspect '" + p.string() + "': " +
             isDirectoryRes.error().message);
        return;
    }

    auto tempRoot = mpc.paths->getDocuments()->tempPath();

    const auto tempRootExistsValue = value(
        mpc_fs::exists(tempRoot), FailurePolicy::Required,
        "Akai rename temp root existence check for '" + tempRoot.string() + "'");
    if (!tempRootExistsValue)
    {
        return;
    }

    if (*tempRootExistsValue)
    {
        const auto removeAllValue = value(
            mpc_fs::remove_all(tempRoot), FailurePolicy::Required,
            "Akai rename temp root cleanup for '" + tempRoot.string() + "'");
        if (!removeAllValue)
        {
            return;
        }
    }

    auto tempRootWasCreated = mpc_fs::create_directory(tempRoot);
    assert(tempRootWasCreated && *tempRootWasCreated);
    if (!tempRootWasCreated || !*tempRootWasCreated)
    {
        if (!tempRootWasCreated)
        {
            logFailure(FailurePolicy::Required,
                       "Akai rename temp root create for '" + tempRoot.string() +
                           "'",
                       tempRootWasCreated.error());
        }
        return;
    }

    std::vector<std::string> existingNames;

    auto dirItRes = mpc_fs::make_directory_iterator(p);
    if (!dirItRes)
    {
        return;
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
            return;
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
                return;
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
        return;
    }

    for (auto e = *tempDirItRes; e != mpc_fs::directory_end(); ++e)
    {
        if (!success(mpc_fs::rename(e->path(), p / e->path().filename()),
                     FailurePolicy::Required,
                     "Akai rename restore from temp root for '" +
                         e->path().string() + "'"))
        {
            return;
        }
    }
}
