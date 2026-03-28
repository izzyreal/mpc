#include "disk/MpcFile.hpp"

#include <fat/AkaiFatLfnDirectoryEntry.hpp>

#include <Logger.hpp>

using namespace mpc::disk;
using namespace akaifat::fat;

MpcFile::MpcFile(
    const std::variant<mpc_fs::path, std::shared_ptr<AkaiFatLfnDirectoryEntry>>
        &fileObject)
{
    if (auto fsPath = std::get_if<mpc_fs::path>(&fileObject))
    {
        fs_path = *fsPath;
        return;
    }
    if (auto akaiEntry =
            std::get_if<std::shared_ptr<AkaiFatLfnDirectoryEntry>>(&fileObject))
    {
        rawEntry = *akaiEntry;
        raw = true;
        return;
    }

    throw std::runtime_error("Invalid object provided to MpcFile");
}

std::vector<std::shared_ptr<MpcFile>> MpcFile::listFiles()
{
    if (!isDirectory())
    {
        return {};
    }

    std::vector<std::shared_ptr<MpcFile>> result;

    if (raw)
    {
        auto dir = std::dynamic_pointer_cast<AkaiFatLfnDirectory>(
            rawEntry->getDirectory());

        for (auto &kv : dir->akaiNameIndex)
        {
            result.emplace_back(std::make_shared<MpcFile>(kv.second));
        }
    }
    else
    {
        const auto pathIteratorRes = mpc_fs::make_directory_iterator(fs_path);
        if (!pathIteratorRes)
        {
            MLOG("MpcFile::listFiles failed for '" + fs_path.string() + "': " +
                 pathIteratorRes.error().message);
            return result;
        }

        std::error_code ec;
        for (auto pathIterator = *pathIteratorRes;
             pathIterator != mpc_fs::directory_end();
             pathIterator.increment(ec))
        {
            if (ec)
            {
                MLOG("MpcFile::listFiles iteration failed for '" +
                     fs_path.string() + "': " + ec.message());
                break;
            }

            auto &path = pathIterator->path();
            std::string filename = path.filename().string();

            if (filename.length() > 0 && filename[0] == '.')
            {
                continue;
            }

            result.emplace_back(std::make_shared<MpcFile>(path));
        }
    }

    return result;
}

std::string MpcFile::getNameWithoutExtension() const
{
    if (raw)
    {
        auto name = rawEntry->getAkaiName();
        auto extIndex = name.find_last_of('.');
        if (extIndex != std::string::npos)
        {
            name = name.substr(0, extIndex);
        }
        return name;
    }
    return fs_path.stem().string();
}

std::string MpcFile::getExtension() const
{
    if (raw)
    {
        auto ext = rawEntry->getAkaiName();
        auto extIndex = ext.find_last_of('.');
        if (extIndex != std::string::npos)
        {
            ext = ext.substr(extIndex);
        }
        else
        {
            ext = "";
        }
        return ext;
    }
    if (fs_path.has_extension())
    {
        return fs_path.extension().string();
    }
    return "";
}

bool MpcFile::isDirectory() const
{
    if (raw)
    {
        return rawEntry->isDirectory();
    }
    const auto isDirectoryRes = mpc_fs::is_directory(fs_path);
    if (!isDirectoryRes)
    {
        MLOG("MpcFile::isDirectory failed for '" + fs_path.string() + "': " +
             isDirectoryRes.error().message);
        return false;
    }
    return *isDirectoryRes;
}

bool MpcFile::isFile()
{
    return !isDirectory();
}

std::string MpcFile::getName() const
{
    if (raw)
    {
        return rawEntry->getAkaiName();
    }
    return fs_path.filename().string();
}

bool MpcFile::setName(const std::string &s) const
{
    if (raw)
    {
        try
        {
            rawEntry->setName(s);
            return true;
        }
        catch (const std::exception &)
        {
            return false;
        }
    }
    mpc_fs::path new_path = fs_path;
    new_path.replace_filename(s);
    const auto renameRes = mpc_fs::rename(fs_path, new_path);
    if (!renameRes)
    {
        MLOG("MpcFile::setName failed from '" + fs_path.string() + "' to '" +
             new_path.string() + "': " + renameRes.error().message);
        return false;
    }
    return true;
}

unsigned long MpcFile::length()
{
    if (isDirectory())
    {
        return 0;
    }
    if (raw)
    {
        try
        {
            return rawEntry->getFile()->getLength();
        }
        catch (std::exception &)
        {
        }
        return 0;
    }
    const auto sizeRes = mpc_fs::file_size(fs_path);
    if (!sizeRes)
    {
        MLOG("MpcFile::length failed for '" + fs_path.string() + "': " +
             sizeRes.error().message);
        return 0;
    }
    return *sizeRes;
}

void MpcFile::setFileData(std::vector<char> &data)
{
    if (raw)
    {
        ByteBuffer bb(data);
        auto f = rawEntry->getFile();
        f->setLength(data.size());
        f->write(0, bb);
        f->flush();
    }
    else
    {
        const auto writeRes = set_file_data(fs_path, data);
        if (!writeRes)
        {
            MLOG("MpcFile::setFileData failed for '" + fs_path.string() + "': " +
                 writeRes.error().message);
        }
    }
}

bool MpcFile::exists() const
{
    if (raw)
    {
        return rawEntry->isValid();
    }
    const auto existsRes = mpc_fs::exists(fs_path);
    if (!existsRes)
    {
        MLOG("MpcFile::exists failed for '" + fs_path.string() + "': " +
             existsRes.error().message);
        return false;
    }
    return *existsRes;
}

bool MpcFile::del() const
{
    if (raw)
    {
        try
        {
            rawEntry->getParent()->remove(rawEntry->getName());
            return true;
        }
        catch (const std::exception &)
        {
            return false;
        }
    }
    const auto removeRes = mpc_fs::remove(fs_path);
    if (!removeRes)
    {
        MLOG("MpcFile::del failed for '" + fs_path.string() + "': " +
             removeRes.error().message);
        return false;
    }
    return *removeRes;
}

std::vector<char> MpcFile::getBytes()
{
    std::vector<char> bytes;

    if (raw)
    {
        try
        {
            ByteBuffer bb(length());
            rawEntry->getFile()->read(0, bb);
            bb.flip();
            while (bb.hasRemaining())
            {
                bytes.emplace_back(bb.get());
            }
        }
        catch (const std::exception &e)
        {
            MLOG("Exception while getting file bytes: " +
                 std::string(e.what()));
        }
    }
    else
    {
        const auto dataRes = get_file_data(fs_path);
        if (!dataRes)
        {
            MLOG("MpcFile::getBytes failed for '" + fs_path.string() + "': " +
                 dataRes.error().message);
            return {};
        }
        bytes = *dataRes;
    }

    return bytes;
}

std::shared_ptr<std::istream> MpcFile::getInputStream()
{
    if (raw)
    {
        return std::dynamic_pointer_cast<FatFile>(rawEntry->getFile())
            ->getInputStream();
    }
    auto inputStream = std::make_shared<std::ifstream>(fs_path,
                                                       std::ios::in | std::ios::binary);
    if (!inputStream->is_open())
    {
        MLOG("MpcFile::getInputStream failed for '" + fs_path.string() + "'");
    }
    return inputStream;
}

std::shared_ptr<std::ostream> MpcFile::getOutputStream()
{
    if (raw)
    {
        return std::dynamic_pointer_cast<FatFile>(rawEntry->getFile())
            ->getOutputStream();
    }
    auto outputStream = std::make_shared<std::ofstream>(fs_path,
                                                        std::ios::out | std::ios::binary);
    if (!outputStream->is_open())
    {
        MLOG("MpcFile::getOutputStream failed for '" + fs_path.string() + "'");
    }
    return outputStream;
}

mpc_fs::path MpcFile::getPath()
{
    return fs_path;
}
