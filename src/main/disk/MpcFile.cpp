#include <disk/MpcFile.hpp>

#include <lang/StrUtil.hpp>
#include <file/FileUtil.hpp>

#include <fat/AkaiFatLfnDirectoryEntry.hpp>

#include <Logger.hpp>

using namespace mpc::disk;
using namespace moduru::lang;
using namespace moduru::file;
using namespace akaifat::fat;

MpcFile::MpcFile(nonstd::any fileObject)
{
    if (!fileObject.has_value()) throw std::runtime_error("No object provided to MpcFile");
    
    if (fileObject.type() == typeid(fs::path))
    {
        fs_path = nonstd::any_cast<fs::path>(fileObject);
    }
    else if (fileObject.type() == typeid(std::shared_ptr<AkaiFatLfnDirectoryEntry>))
    {
        rawEntry = nonstd::any_cast<std::shared_ptr<AkaiFatLfnDirectoryEntry>>(fileObject);
        raw = true;
    }
    else throw std::runtime_error("Invalid object provided to MpcFile");
}

std::vector<std::shared_ptr<MpcFile>> MpcFile::listFiles()
{
    if (!isDirectory()) return {};
    
    std::vector<std::shared_ptr<MpcFile>> result;
    
    if (raw) {
        auto dir = std::dynamic_pointer_cast<AkaiFatLfnDirectory>(rawEntry->getDirectory());
        
        for (auto& kv : dir->akaiNameIndex)
            result.emplace_back(std::make_shared<MpcFile>(kv.second));
    } else {
        std::error_code ec;
        for(auto pathIterator = fs::directory_iterator(fs_path, ec);
            pathIterator != fs::directory_iterator(); pathIterator.increment(ec))
        {
            if(!ec) {
                auto& path = pathIterator->path();
                std::string filename = path.filename();
                
                if (filename.length() > 0 && filename[0] == '.')
                    continue;
                
                result.emplace_back(std::make_shared<MpcFile>(path));
            }
        }
    }
    
    return result;
}

std::string MpcFile::getNameWithoutExtension()
{
    if (raw) {
        auto name = rawEntry->getName();
        auto extIndex = name.find_last_of('.');
        if (extIndex != std::string::npos) {
            name = name.substr(0, extIndex);
        }
        return name;
    }
    else {
        return fs_path.stem().string();
    }
}

bool MpcFile::isDirectory()
{
    if (raw)
        return rawEntry->isDirectory();
    else
        return fs::is_directory(fs_path);
}

bool MpcFile::isFile()
{
    return !isDirectory();
}

std::string MpcFile::getName()
{
    if (raw)
        return rawEntry->getName();
    else
        return fs_path.filename();
}

bool MpcFile::setName(std::string s)
{
    if (raw) {
        try {
            rawEntry->setName(s);
            return true;
        }
        catch (const std::exception&) {
            return false;
        }
    }
    else {
//        return stdNode->renameTo(s);
        return false;
    }
}

int MpcFile::length()
{
    if (isDirectory()) return 0;
    if (raw) {
        try {
            return rawEntry->getFile()->getLength();
        }
        catch (std::exception&) {}
        return 0;
    }
    else
        return fs::file_size(fs_path);
}

void MpcFile::setFileData(std::vector<char>* data)
{
    if (raw) {
        ByteBuffer bb(*data);
        auto f = rawEntry->getFile();
        f->setLength(data->size());
        f->write(0, bb);
        f->flush();
    }
    else {
        //std::dynamic_pointer_cast<moduru::file::File>(stdNode)->setData(data);
    }
}

bool MpcFile::exists()
{
    if (raw) {
        return rawEntry->isValid();
    }
    else {
        return fs::exists(fs_path);
    }
}

bool MpcFile::del()
{
    if (raw) {
        try {
            rawEntry->getParent()->remove(rawEntry->getName());
            return true;
        }
        catch (const std::exception&) {
            return false;
        }
    }
    else {
        return false;
    }
}

std::vector<char> MpcFile::getBytes()
{
    std::vector<char> bytes;
    
    if (raw) {
        try {
            ByteBuffer bb(length());
            rawEntry->getFile()->read(0, bb);
            bb.flip();
            while (bb.hasRemaining()) bytes.emplace_back(bb.get());
        } catch (const std::exception& e) {
            MLOG("Exception while getting file bytes: " + std::string(e.what()));
        }
    }
    else {
        auto ostream = getInputStream();
        bytes.resize(length());
        ostream->read(&bytes[0], length());
    }
    
    return bytes;
}

std::shared_ptr<std::istream> MpcFile::getInputStream()
{
    if (raw) {
        return std::dynamic_pointer_cast<akaifat::fat::FatFile>(rawEntry->getFile())->getInputStream();
    } else {
        return std::make_shared<std::ifstream>(std::move(FileUtil::ifstreamw(fs_path, std::ios::in | std::ios::binary)));
    }
}

std::shared_ptr<std::ostream> MpcFile::getOutputStream()
{
    if (raw) {
        return std::dynamic_pointer_cast<akaifat::fat::FatFile>(rawEntry->getFile())->getOutputStream();
    } else {
        return std::make_shared<std::ofstream>(std::move(FileUtil::ofstreamw(fs_path, std::ios::out | std::ios::binary)));
    }
}
