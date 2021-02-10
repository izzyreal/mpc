#include "AbstractDisk.hpp"

#include <Mpc.hpp>
#include <Util.hpp>
#include <disk/MpcFile.hpp>
#include <file/wav/WavFile.hpp>
#include <file/mid/MidiWriter.hpp>
#include <file/pgmwriter/PgmWriter.hpp>
#include <file/sndwriter/SndWriter.hpp>

#include <sampler/NoteParameters.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/LoadScreen.hpp>
#include <lcdgui/screens/window/DirectoryScreen.hpp>
#include <lcdgui/screens/window/SaveAProgramScreen.hpp>

#include <file/FileUtil.hpp>
#include <lang/StrUtil.hpp>

using namespace mpc::disk;

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

using namespace mpc::sampler;

using namespace moduru::lang;
using namespace moduru::file;

using namespace std;

AbstractDisk::AbstractDisk(mpc::Mpc& mpc, weak_ptr<mpc::disk::Store> store)
	: mpc(mpc)
{
	this->store = store;
}

void AbstractDisk::setBusy(bool b)
{
	busy = b;
}

bool AbstractDisk::isBusy()
{
	return busy;
}

string AbstractDisk::formatFileSize(int size)
{
	string hrSize = "";
	float b = size;
	float k = size / 1024.0;
	float m = ((size / 1024.0) / 1024.0);
	float g = (((size / 1024.0) / 1024.0) / 1024.0);
	float t = ((((size / 1024.0) / 1024.0) / 1024.0) / 1024.0);

	if(t > 1)
	{
		hrSize = StrUtil::TrimDecimals(t, 2) + " TB";
	} else if(g > 1)
	{
		hrSize = StrUtil::TrimDecimals(g, 2) + " GB";
	} else if(m > 1)
	{
		hrSize = StrUtil::TrimDecimals(m, 2) + " MB";
	} else if(k > 1)
	{
		hrSize = StrUtil::TrimDecimals(k, 2) + " KB";
	} else
	{
		hrSize = StrUtil::TrimDecimals(b, 2) + " Bytes";
	}
	return hrSize;
}

string AbstractDisk::padFileName16(string s)
{
	if (s.find(".") == string::npos) return s;
	auto periodIndex = s.find_last_of(".");
	auto name = s.substr(0, periodIndex);
	auto ext = s.substr(periodIndex);
	name = StrUtil::padRight(name, " ", 16);
	return name + ext;
}

shared_ptr<MpcFile> AbstractDisk::getFile(int i)
{
	return files[i];
}

vector<string> AbstractDisk::getFileNames()
{
	vector<string> res;
	transform(files.begin(), files.end(), back_inserter(res), [](shared_ptr<MpcFile> f) { return f->getName(); });
	return res;
}

string AbstractDisk::getFileName(int i)
{
	return files[i]->getName();
}

vector<string> AbstractDisk::getParentFileNames()
{
	vector<string> res;

	for (auto& f : parentFiles)
		res.push_back(f->getName().length() < 8 ? f->getName() : f->getName().substr(0, 8));

	return res;
}

bool AbstractDisk::renameSelectedFile(string s)
{
	auto directoryScreen = mpc.screens->get<DirectoryScreen>("directory");
	auto loadScreen = mpc.screens->get<LoadScreen>("load");
  
	auto left = directoryScreen->xPos == 0;
	auto fileIndex = left ? directoryScreen->yPos0 + directoryScreen->yOffset0 : loadScreen->fileLoad;

    auto file = left ? getParentFile(fileIndex) : getFile(fileIndex);
    return file->setName(s);
}

bool AbstractDisk::deleteSelectedFile()
{
	auto loadScreen = mpc.screens->get<LoadScreen>("load");
	return files[loadScreen->fileLoad]->del();
}

vector<shared_ptr<MpcFile>>& AbstractDisk::getFiles()
{
	return files;
}

vector<shared_ptr<MpcFile>>& AbstractDisk::getAllFiles() {
	return allFiles;
}

shared_ptr<MpcFile> AbstractDisk::getParentFile(int i)
{
    return parentFiles[i];
}

vector<shared_ptr<MpcFile>>& AbstractDisk::getParentFiles()
{
	return parentFiles;
}

void AbstractDisk::writeSound(weak_ptr<Sound> s)
{
	string name = mpc::Util::getFileName(s.lock()->getName() + ".SND");
    auto nf = newFile(name);
    writeSound(s, nf);
}

void AbstractDisk::writeWav(weak_ptr<Sound> s)
{
	string name = mpc::Util::getFileName(s.lock()->getName() + ".WAV");
	auto nf = newFile(name);
	writeWav(s, nf);
}

void AbstractDisk::writeSound(weak_ptr<Sound> s, weak_ptr<MpcFile> f)
{
	auto sw = mpc::file::sndwriter::SndWriter(s.lock().get());
	auto sndArray = sw.getSndFileArray();
	f.lock()->setFileData(&sndArray);
	flush();
	initFiles();
}

void AbstractDisk::close()
{
	device->close();
}

void AbstractDisk::flush()
{
	device->flush();
}

weak_ptr<mpc::disk::Store> AbstractDisk::getStore()
{
	return store;
}


void AbstractDisk::writeWav(weak_ptr<Sound> s, weak_ptr<MpcFile> f)
{
    auto sound = s.lock();
    auto data = sound->getSampleData();
    auto isMono = sound->isMono();
    
    auto wavFile = mpc::file::wav::WavFile::newWavFile(f.lock()->getFile().lock()->getPath(), isMono ? 1 : 2, data->size() / (isMono ? 1 : 2), 16, sound->getSampleRate());
    
    if (isMono)
    {
        wavFile.writeFrames(data, data->size());
    }
    else
    {
        vector<float> interleaved;
        
        for (int i = 0; i < (int) (data->size() * 0.5); i++)
        {
            interleaved.push_back((*data)[i]);
            interleaved.push_back((*data)[(int) (i + data->size() * 0.5)]);
        }
        
        wavFile.writeFrames(&interleaved, data->size() * 0.5);
    }
    
    wavFile.close();
    flush();
    initFiles();
}

void AbstractDisk::writeSequence(weak_ptr<mpc::sequencer::Sequence> s, string fileName)
{
	if (checkExists(fileName))
		return;
	
	auto newMidFile = newFile(fileName);
	
	auto writer = mpc::file::mid::MidiWriter(s.lock().get());
	writer.writeToFile(newMidFile->getFile().lock()->getPath());

	flush();
	initFiles();
}

bool AbstractDisk::checkExists(string fileName)
{
	initFiles();

	auto fileNameSplit = FileUtil::splitName(fileName);

	for (auto& file : getAllFiles())
	{
		auto name = FileUtil::splitName(file->getName());
		auto nameIsSame = StrUtil::eqIgnoreCase(name[0], fileNameSplit[0]);
		auto extIsSame = StrUtil::eqIgnoreCase(name[1], fileNameSplit[1]);
		
		if (nameIsSame && extIsSame)
			return true;
	}

	return false;
}

shared_ptr<MpcFile> AbstractDisk::getFile(const string& fileName)
{
	auto tempfileName = StrUtil::replaceAll(fileName, ' ', "");
	
    for (auto& f : files)
    {
		if (StrUtil::eqIgnoreCase(StrUtil::replaceAll(f->getName(), ' ', ""), tempfileName))
			return f;
	}
	
    for (auto& f : allFiles)
    {
		if (StrUtil::eqIgnoreCase(StrUtil::replaceAll(f->getName(), ' ', ""), tempfileName))
			return f;
	}
    
    return {};
}

void AbstractDisk::writeProgram(weak_ptr<Program> program, const string& fileName)
{
	if (checkExists(fileName))
		return;

	auto writer = mpc::file::pgmwriter::PgmWriter(program.lock().get(), mpc.getSampler());
	auto pgmFile = newFile(fileName);
    auto bytes = writer.get();
	pgmFile->setFileData(&bytes);
	vector<weak_ptr<Sound>> sounds;

	for (auto& n : program.lock()->getNotesParameters())
	{
		if (n->getSoundIndex() != -1)
            sounds.push_back(mpc.getSampler().lock()->getSound(n->getSoundIndex()).lock());
	}

	auto saveAProgramScreen = mpc.screens->get<SaveAProgramScreen>("save-a-program");
	
	if (saveAProgramScreen->save != 0)
	{
		auto isWav = saveAProgramScreen->save == 2;
		soundSaver = make_unique<SoundSaver>(mpc, sounds, isWav);
	}
	else
    {
		mpc.getLayeredScreen().lock()->openScreen("save");
	}

	flush();
	initFiles();
	setBusy(false);
}

bool AbstractDisk::isRoot()
{
	return getPathDepth() == 0;
}
