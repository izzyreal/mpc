#include "AkaiFileRenamer.hpp"

#include "Mpc.hpp"
#include "Logger.hpp"

#include <cassert>

#include "ShortNameGenerator.hpp"
#include "ShortName.hpp"
#include "file/AkaiName.hpp"

using namespace mpc::disk;
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

    const auto tempRootExistsRes = mpc_fs::exists(tempRoot);
    if (!tempRootExistsRes)
    {
        MLOG("AkaiFileRenamer: failed to inspect temp root '" +
             tempRoot.string() + "': " + tempRootExistsRes.error().message);
        return;
    }

    if (*tempRootExistsRes)
    {
        const auto removeAllRes = mpc_fs::remove_all(tempRoot);
        if (!removeAllRes)
        {
            MLOG("AkaiFileRenamer: failed to clear temp root '" +
                 tempRoot.string() + "': " + removeAllRes.error().message);
            return;
        }
    }

    auto tempRootWasCreated = mpc_fs::create_directory(tempRoot);
    assert(tempRootWasCreated && *tempRootWasCreated);
    if (!tempRootWasCreated)
    {
        MLOG("AkaiFileRenamer: failed to create temp root '" +
             tempRoot.string() + "': " + tempRootWasCreated.error().message);
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
            const auto renameRes = mpc_fs::rename(e->path(), tempRoot / akaiName);
            if (!renameRes)
            {
                MLOG("AkaiFileRenamer: failed moving '" + e->path().string() +
                     "' to temp root: " + renameRes.error().message);
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
        const auto renameRes = mpc_fs::rename(e->path(), p / e->path().filename());
        if (!renameRes)
        {
            MLOG("AkaiFileRenamer: failed restoring '" + e->path().string() +
                 "' from temp root: " + renameRes.error().message);
            return;
        }
    }
}
