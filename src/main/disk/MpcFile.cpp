#include "disk/MpcFile.hpp"

#include <StrUtil.hpp>

#include <fat/AkaiFatLfnDirectoryEntry.hpp>

#include <Logger.hpp>

using namespace mpc::disk;
using namespace akaifat::fat;

MpcFile::MpcFile(
    const std::variant<fs::path,
                       std::shared_ptr<akaifat::fat::AkaiFatLfnDirectoryEntry>>
        &fileObject)
{
    if (auto fsPath = std::get_if<fs::path>(&fileObject))
    {
        fs_path = *fsPath;
        return;
    }
    else if (auto akaiEntry =
                 std::get_if<std::shared_ptr<AkaiFatLfnDirectoryEntry>>(
                     &fileObject))
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
        std::error_code ec;
        for (auto pathIterator = fs::directory_iterator(fs_path, ec);
             pathIterator != fs::directory_iterator();
             pathIterator.increment(ec))
        {
            if (!ec)
            {
                auto &path = pathIterator->path();
                std::string filename = path.filename().string();

                if (filename.length() > 0 && filename[0] == '.')
                {
                    continue;
                }

                result.emplace_back(std::make_shared<MpcFile>(path));
            }
        }
    }

    return result;
}

std::string MpcFile::getNameWithoutExtension()
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
    else
    {
        return fs_path.stem().string();
    }
}

std::string MpcFile::getExtension()
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
    else
    {
        if (fs_path.has_extension())
        {
            return fs_path.extension().string();
        }
        return "";
    }
}

bool MpcFile::isDirectory()
{
    if (raw)
    {
        return rawEntry->isDirectory();
    }
    else
    {
        return fs::is_directory(fs_path);
    }
}

bool MpcFile::isFile()
{
    return !isDirectory();
}

std::string MpcFile::getName()
{
    if (raw)
    {
        return rawEntry->getAkaiName();
    }
    else
    {
        return fs_path.filename().string();
    }
}

bool MpcFile::setName(std::string s)
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
    else
    {
        fs::path new_path = fs_path;
        new_path.replace_filename(s);
        std::error_code ec;
        fs::rename(fs_path, new_path, ec);
        return ec.value() == 0;
    }
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
    else
    {
        return static_cast<unsigned long>(fs::file_size(fs_path));
    }
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
        auto ostream = getOutputStream();
        ostream->write(&data[0], data.size());
    }
}

bool MpcFile::exists()
{
    if (raw)
    {
        return rawEntry->isValid();
    }
    else
    {
        return fs::exists(fs_path);
    }
}

bool MpcFile::del()
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
    else
    {
        return fs::remove(fs_path);
    }
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
        auto istream = getInputStream();
        bytes.resize(length());
        istream->read(&bytes[0], length());
    }

    return bytes;
}

std::shared_ptr<std::istream> MpcFile::getInputStream()
{
    if (raw)
    {
        return std::dynamic_pointer_cast<akaifat::fat::FatFile>(
                   rawEntry->getFile())
            ->getInputStream();
    }
    else
    {
        return std::make_shared<std::ifstream>(fs_path,
                                               std::ios::in | std::ios::binary);
    }
}

std::shared_ptr<std::ostream> MpcFile::getOutputStream()
{
    if (raw)
    {
        return std::dynamic_pointer_cast<akaifat::fat::FatFile>(
                   rawEntry->getFile())
            ->getOutputStream();
    }
    else
    {
        return std::make_shared<std::ofstream>(fs_path, std::ios::out |
                                                            std::ios::binary);
    }
}

fs::path MpcFile::getPath()
{
    return fs_path;
}