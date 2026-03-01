#include "AkaiFileRenamer.hpp"

#include "Mpc.hpp"

#include <cassert>

#include "ShortNameGenerator.hpp"
#include "ShortName.hpp"
#include "file/AkaiName.hpp"

using namespace mpc::disk;
using namespace mpc::file;

void AkaiFileRenamer::renameFilesInDirectory(Mpc &mpc, const mpc_fs::path &p)
{
    assert(mpc_fs::is_directory(p).value_or(false));

    auto tempRoot = mpc.paths->getDocuments()->tempPath();

    if (mpc_fs::exists(tempRoot).value_or(false))
    {
        (void) mpc_fs::remove_all(tempRoot);
    }

    auto tempRootWasCreated = mpc_fs::create_directory(tempRoot);
    assert(tempRootWasCreated && *tempRootWasCreated);

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

        if (mpc_fs::is_directory(*e).value_or(false))
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
            (void) mpc_fs::rename(e->path(), tempRoot / akaiName);
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
        (void) mpc_fs::rename(e->path(), p / e->path().filename());
    }
}
