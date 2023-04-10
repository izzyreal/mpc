#include "AkaiFileRenamer.hpp"

#include <cassert>

#include <raw/fat/ShortNameGenerator.hpp>
#include <raw/fat/ShortName.hpp>
#include "../file/AkaiName.hpp"

using namespace mpc::disk;
using namespace mpc::file;
using namespace moduru::raw::fat;

void AkaiFileRenamer::renameFilesInDirectory(fs::path p)
{
    assert(fs::is_directory(p));

    auto tempRoot = fs::temp_directory_path() / "VMPC2000XL";

    if (fs::exists(tempRoot))
    {
        fs::remove_all(tempRoot);
    }

    auto tempRootWasCreated = fs::create_directory(tempRoot);
    assert(tempRootWasCreated);

    std::vector<std::string> existingNames;

    for (auto& e : fs::directory_iterator(p))
    {
        std::string akaiName;

        if (fs::is_directory(e))
        {
            const auto tidyString = ShortNameGenerator::tidyString(e.path().filename().string());
            ShortNameGenerator generator(existingNames);
            akaiName = generator.generateShortName(tidyString).asSimpleString();
        }
        else
        {
            akaiName = AkaiName::generate(e.path().filename().string(), existingNames);
        }

        if (akaiName != e.path().filename().string())
        {
            existingNames.push_back(akaiName);
            fs::rename(e, tempRoot / akaiName);
        }
        else
        {
            existingNames.push_back(e.path().filename().string());
        }
    }

    for (auto& e : fs::directory_iterator(tempRoot))
    {
        fs::rename(e, p / e.path().filename());
    }
}
