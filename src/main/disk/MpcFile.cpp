#include <disk/MpcFile.hpp>

#include <file/Directory.hpp>
#include <lang/StrUtil.hpp>
#include <file/FsNode.hpp>
#include <file/File.hpp>

#include <fat/AkaiFatLfnDirectoryEntry.hpp>

#include <Logger.hpp>

using namespace mpc::disk;
using namespace moduru::lang;
using namespace moduru::file;
using namespace akaifat::fat;

MpcFile::MpcFile(nonstd::any fileObject)
{
    if (fileObject.has_value())
    {
        
        try {
            if (fileObject.type() == typeid(std::shared_ptr<File>))
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
        }
        catch (const std::exception& e) {
            MLOG("Failed to cast fileObject to shared_ptr<FsNode>: " + std::string(e.what()));
        }
    }
}

bool MpcFile::isDirectory()
{
    if (raw) {
        return rawEntry->isDirectory();
    }
    else {
        auto dir = std::dynamic_pointer_cast<Directory>(getFsNode().lock());
        if (dir) {
            return true;
        }
        else {
            return false;
        }
    }
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

std::weak_ptr<moduru::file::FsNode> MpcFile::getFsNode()
{
//    if (raw)
//        return RawDisk::entryToFile(rawEntry);
//    else
        return stdNode;
}

std::weak_ptr<moduru::file::File> MpcFile::getFile() {
    return std::dynamic_pointer_cast<moduru::file::File>(stdNode);
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

bool MpcFile::isStd()
{
    return stdNode != nullptr;
}
