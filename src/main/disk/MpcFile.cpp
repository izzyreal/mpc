#include <disk/MpcFile.hpp>

#include <file/Directory.hpp>
#include <lang/StrUtil.hpp>
#include <file/FsNode.hpp>
#include <file/File.hpp>
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
    
    if (fileObject.type() == typeid(std::shared_ptr<Directory>))
    {
        stdNode = nonstd::any_cast<std::shared_ptr<Directory>>(fileObject);
    }
    else if (fileObject.type() == typeid(std::shared_ptr<File>))
    {
        stdNode = nonstd::any_cast<std::shared_ptr<File>>(fileObject);
    }
    else if (fileObject.type() == typeid(std::shared_ptr<FsNode>))
    {
        stdNode = nonstd::any_cast<std::shared_ptr<FsNode>>(fileObject);
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
        
        auto dir = std::dynamic_pointer_cast<Directory>(stdNode);
        
        for (auto& node : dir->listFiles())
            result.emplace_back(std::make_shared<MpcFile>(node));
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
        return stdNode->getNameWithoutExtension();
    }
}

bool MpcFile::isDirectory()
{
    if (raw)
        return rawEntry->isDirectory();
    else
        return stdNode->isDirectory();
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
        return stdNode->getName();
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
        return stdNode->renameTo(s);
    }
}

int MpcFile::length()
{
    if (raw) {
        auto length = 0;
        try {
            if (rawEntry->isFile())
                return rawEntry->getFile()->getLength();
        }
        catch (std::exception&) {
            return 0;
        }
        return length;
    }
    else
        return stdNode->getLength();
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
        std::dynamic_pointer_cast<moduru::file::File>(stdNode)->setData(data);
    }
}

bool MpcFile::exists()
{
    if (raw) {
        return rawEntry->isValid();
    }
    else {
        return stdNode->exists();
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
        return stdNode->del();
    }
}

std::vector<char> MpcFile::getBytes()
{
    std::vector<char> bytes(length());
    
    if (raw) {
        try {
            ByteBuffer bb(bytes);
            rawEntry->getFile()->read(0, bb);
        } catch (const std::exception& e) {
            MLOG("Exception while getting file bytes: " + std::string(e.what()));
        }
    }
    else {
        try {
            std::dynamic_pointer_cast<moduru::file::File>(stdNode)->getData(&bytes);
        } catch (std::exception& e) {
            MLOG("Exception while getting file bytes: " + std::string(e.what()));
        }
    }
    
    return bytes;
}

std::shared_ptr<std::istream> MpcFile::getInputStream()
{
    if (raw) {
        return std::dynamic_pointer_cast<akaifat::fat::FatFile>(rawEntry->getFile())->getInputStream();
    } else {
        return std::make_shared<std::ifstream>(std::move(FileUtil::ifstreamw(stdNode->getPath(), std::ios::in | std::ios::binary)));
    }
}

std::shared_ptr<std::ostream> MpcFile::getOutputStream()
{
    if (raw) {
        return std::dynamic_pointer_cast<akaifat::fat::FatFile>(rawEntry->getFile())->getOutputStream();
    } else {
        return std::make_shared<std::ofstream>(std::move(FileUtil::ofstreamw(stdNode->getPath(), std::ios::out | std::ios::binary)));
    }
}
