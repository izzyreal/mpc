#include "SaveDestination.hpp"
#include "StrUtil.hpp"
#include <algorithm>
#include <fat/AkaiFatLfnDirectory.hpp>
#include <fat/AkaiFatLfnDirectoryEntry.hpp>
#include <stdexcept>

using namespace mpc::disk;

SaveDestination::SaveDestination(
    std::function<std::vector<std::shared_ptr<MpcFile>>()> listCurrent,
    std::function<std::vector<std::shared_ptr<MpcFile>>()> listParent,
    std::function<std::shared_ptr<MpcFile>(const std::string &)> createFile,
    bool sorted, bool writable, int view, std::function<void()> normalize,
    std::function<void()> flushVolume)
    : listCurrent(std::move(listCurrent)), listParent(std::move(listParent)),
      createFile(std::move(createFile)), sorted(sorted), writable(writable),
      view(view), normalize(std::move(normalize)),
      flushVolume(std::move(flushVolume))
{
}

void SaveDestination::prepare()
{
    if (!writable)
    {
        throw std::runtime_error("Disk is read only");
    }
    normalize();
    entries = listCurrent();
}

std::shared_ptr<MpcFile> SaveDestination::find(const std::string &name) const
{
    const auto path = mpc_fs::path(name);
    for (const auto &entry : entries)
    {
        const auto candidate = mpc_fs::path(entry->getName());
        if (mpc::StrUtil::eqIgnoreCase(candidate.stem().string(),
                                       path.stem().string()) &&
            mpc::StrUtil::eqIgnoreCase(candidate.extension().string(),
                                       path.extension().string()))
        {
            return entry;
        }
    }
    return {};
}

std::shared_ptr<MpcFile> SaveDestination::create(const std::string &name,
                                                 bool replace)
{
    if (const auto existing = find(name))
    {
        if (!replace || existing->isDirectory() || !existing->del())
        {
            throw std::runtime_error("Unable to replace " + name);
        }
        entries.erase(std::remove(entries.begin(), entries.end(), existing),
                      entries.end());
    }
    auto result = createFile(name);
    entries.push_back(result);
    return result;
}

void SaveDestination::remove(const std::shared_ptr<MpcFile> &file)
{
    if (file->isDirectory() || !file->del())
    {
        throw std::runtime_error("Unable to replace " + file->getName());
    }
    entries.erase(std::remove(entries.begin(), entries.end(), file),
                  entries.end());
}

void SaveDestination::flush()
{
    flushVolume();
}

DirectoryListing SaveDestination::scan()
{
    // Match legacy refresh normalization, but never publish from the worker.
    normalize();
    DirectoryListing result;
    result.allFiles = listCurrent();
    static const std::vector<std::string> extensions{
        "", "SND", "PGM", "APS", "MID", "ALL", "WAV", "SEQ", "SET"};
    for (const auto &entry : result.allFiles)
    {
        const auto name = entry->getName();
        if (view == 0 || entry->isDirectory() ||
            (view > 0 && view < extensions.size() && name.size() >= 3 &&
             name.find('.') != std::string::npos &&
             name.substr(name.size() - 3) == extensions[view]))
        {
            result.files.push_back(entry);
        }
    }
    if (listParent)
    {
        for (const auto &entry : listParent())
        {
            if (entry->isDirectory())
            {
                result.parentFiles.push_back(entry);
            }
        }
    }
    if (sorted)
    {
        const auto byName = [](const auto &a, const auto &b)
        {
            return a->getName() < b->getName();
        };
        std::sort(result.files.begin(), result.files.end(), byName);
        std::stable_partition(result.files.begin(), result.files.end(),
                              [](const auto &f)
                              {
                                  return f->isDirectory();
                              });
        std::sort(result.parentFiles.begin(), result.parentFiles.end(), byName);
    }
    return result;
}

std::unique_ptr<SaveDestination> SaveDestination::fromRaw(
    std::shared_ptr<akaifat::fat::AkaiFatLfnDirectory> directory,
    std::shared_ptr<akaifat::fat::AkaiFatLfnDirectory> parent, bool writable,
    int view, std::function<void()> flush)
{
    using namespace akaifat::fat;
    const auto list = [](const std::shared_ptr<AkaiFatLfnDirectory> &dir)
    {
        std::vector<std::shared_ptr<MpcFile>> result;
        if (dir)
        {
            for (const auto &[name, entry] : dir->akaiNameIndex)
            {
                if (!name.empty() && name[0] != '.' && entry->isValid())
                {
                    result.push_back(std::make_shared<MpcFile>(entry));
                }
            }
        }
        return result;
    };
    return std::make_unique<SaveDestination>(
        [directory, list]
        {
            if (!directory)
            {
                throw std::runtime_error("No save destination");
            }
            return list(directory);
        },
        [parent, list]
        {
            return list(parent);
        },
        [directory](const std::string &name)
        {
            auto normalized =
                StrUtil::toUpper(StrUtil::replaceAll(name, ' ', "_"));
            auto entry = std::dynamic_pointer_cast<AkaiFatLfnDirectoryEntry>(
                directory->addFile(normalized));
            if (!entry)
            {
                throw std::runtime_error("Unable to create " + normalized);
            }
            return std::make_shared<MpcFile>(entry);
        },
        false, writable, view, [] {}, std::move(flush));
}
