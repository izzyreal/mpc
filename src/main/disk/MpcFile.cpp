#include <disk/MpcFile.hpp>

//#include <disk/RawDisk.hpp>
//#include <de/waldheinz/fs/FsDirectory.hpp>
//#include <de/waldheinz/fs/fat/AkaiFatLfnDirectoryEntry.hpp>
//#include <de/waldheinz/fs/fat/FatFile.hpp>

#include <file/Directory.hpp>
#include <lang/StrUtil.hpp>
#include <file/FsNode.hpp>
#include <file/File.hpp>
#include <io/FileOutputStream.hpp>

using namespace mpc::disk;
using namespace moduru::lang;
using namespace moduru::file;
using namespace std;

MpcFile::MpcFile(nonstd::any fileObject)
{
	//raw = dynamic_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* >(fileObject) != nullptr;
	try {
		stdEntry = nonstd::any_cast<shared_ptr<FsNode>>(fileObject);
		std = true;
	}
	catch (std::exception e) {
		e.what();
		std = false;
	}
	//rawEntry = raw ? java_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* >(fileObject) : static_cast< ::de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* >(nullptr);
}

bool MpcFile::isDirectory()
{
	if (raw) {
		//return rawEntry->isDirectory();
		return false;
	}
	else {
		auto dir = dynamic_pointer_cast<Directory>(getFsNode().lock());
		if (dir) {
			return true;
		}
		else {
			return false;
		}
	}
}

string MpcFile::getName()
{
	if (raw) {
		//return rawEntry->getName()->replaceAll(u" "_j, u""_j)->toUpperCase();
		return "";
	}
	else
		return stdEntry->getName();
}

bool MpcFile::setName(string s)
{
	if (raw) {
		try {
			//rawEntry->setName(npc(s)->toUpperCase());
			return true;
		}
		catch (exception e) {
			return false;
		}
	}
	else {
		return stdEntry->renameTo(s);
	}
}

int MpcFile::length()
{
	if (raw) {
		auto length = 0;
		try {
			//length = static_cast< int >(npc(entry->getFsNode())->getLength());
		}
		catch (exception e) {
			//e->printStackTrace();
			return 0;
		}
		return length;
	}
	else
		return stdEntry->getLength();
}

void MpcFile::setFileData(std::vector<char>* data)
{
	if (raw) {
        /*
		auto toWrite = data->size();
		auto sectorSize = 512;
		auto written = 0;
		auto sector = 0;
		auto block = vector<char>(sectorSize);
		try {
			while (toWrite - written > sectorSize) {
				::java::lang::System::arraycopy(data, sector * sectorSize, block, 0, sectorSize);
				npc(entry->getFsNode())->write(static_cast< int >(sector++ * sectorSize), ::java::nio::ByteBuffer::wrap(block));
				written += sectorSize;
			}
			auto remaining = toWrite - written;
			block = ::int8_tArray(remaining);
			::java::lang::System::arraycopy(data, sector * sectorSize, block, 0, remaining);
			npc(entry->getFsNode())->write(static_cast< int >(sector * sectorSize), ::java::nio::ByteBuffer::wrap(block));
			npc(entry->getFsNode())->flush();
		} catch (::java::lang::Exception* e) {
			e->printStackTrace();
		}
		*/
	}
	else {
		auto fos = make_unique<moduru::io::FileOutputStream>(dynamic_pointer_cast<moduru::file::File>(stdEntry).get());
		fos->writeBytes(*data, 0, data->size());
		fos->close();
	}
}

bool MpcFile::del()
{
	if (raw) {
		try {
			//rawEntry->getParent()->remove(rawEntry->getName());
			return true;
		}
		catch (exception e) {
			//e->printStackTrace();
			return false;
		}
	}
	else {
		return stdEntry->del();
	}
}

weak_ptr<moduru::file::FsNode> MpcFile::getFsNode()
{
    //if(raw)
      //  return RawDisk::entryToFile(rawEntry);
    //else
        return stdEntry;
}

weak_ptr<moduru::file::File> MpcFile::getFile() {
	return dynamic_pointer_cast<moduru::file::File>(stdEntry);
}

/*
de::waldheinz::fs::fat::AkaiFatLfnDirectoryEntry* MpcFile::getEntry()
{
    if(!raw)
        return nullptr;

    return rawEntry;
}
*/

vector<char> MpcFile::getBytes()
{
	vector<char> bytes;
    if(raw) {
        int const toRead = length();
		int const sectorSize = 512;
		vector<char> byteList;
        auto sector = 0;
        auto read = 0;
        /*
		auto bb = ByteBuffer::allocate(sectorSize);
        try {
            while (toRead - read > sectorSize) {
                npc(bb)->clear();
                npc(entry->getFsNode())->read(static_cast< int >(sector++ * sectorSize), bb);
                npc(bb)->position(0);
                for (int i = 0; i < sectorSize; i++) 
                                        npc(byteList)->add(::java::lang::Byte::valueOf(npc(bb)->get()));

                read += sectorSize;
            }
            auto const remaining = toRead - read;
            bb = ::java::nio::ByteBuffer::allocate(remaining);
            npc(entry->getFsNode())->read(static_cast< int >(sector * sectorSize), bb);
            npc(bb)->position(0);
            for (int i = 0; i < remaining; i++) 
                                npc(byteList)->add(::java::lang::Byte::valueOf(npc(bb)->get()));

        } catch (::java::io::IOException* e) {
            e->printStackTrace();
        }
        for (int i = 0; i < bytes->length; i++) 
                        (*bytes)[i] = (npc(java_cast< ::java::lang::Byte* >(npc(byteList)->get(i))))->byteValue();

		*/
        return bytes;
    }

    try {
		bytes = vector<char>(length());
		dynamic_pointer_cast<moduru::file::File>(stdEntry)->getData(&bytes);
    } catch (exception e) {
		e.what();
    }
    return bytes;
}

bool MpcFile::isStd() {
	return std;
}

MpcFile::~MpcFile() {
}
