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
    assert(mpc_fs::is_directory(p));

    auto tempRoot = mpc.paths->getDocuments()->tempPath();

    if (mpc_fs::exists(tempRoot))
    {
        mpc_fs::remove_all(tempRoot);
    }

    auto tempRootWasCreated = mpc_fs::create_directory(tempRoot);
    assert(tempRootWasCreated);

    std::vector<std::string> existingNames;

    for (auto &e : mpc_fs::directory_iterator(p))
    {
        std::string akaiName;

        if (e.path().filename() == ".DS_Store")
        {
            continue;
        }

        if (mpc_fs::is_directory(e))
        {
            const auto tidyString =
                ShortNameGenerator::tidyString(e.path().filename().string());
            ShortNameGenerator generator(existingNames);
            akaiName = generator.generateShortName(tidyString).asSimpleString();
        }
        else
        {
            akaiName =
                AkaiName::generate(e.path().filename().string(), existingNames);
        }

        if (akaiName != e.path().filename().string())
        {
            existingNames.push_back(akaiName);
            mpc_fs::rename(e, tempRoot / akaiName);
        }
        else
        {
            existingNames.push_back(e.path().filename().string());
        }
    }

    for (auto &e : mpc_fs::directory_iterator(tempRoot))
    {
        mpc_fs::rename(e, p / e.path().filename());
    }
}
