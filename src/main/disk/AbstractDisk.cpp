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

MpcFile* AbstractDisk::getFile(int i)
{
	return files[i];
}

vector<string> AbstractDisk::getFileNames()
{
	vector<string> res;
	transform(files.begin(), files.end(), back_inserter(res), [](MpcFile* f) { return f->getName(); });
	return res;
}

string AbstractDisk::getFileName(int i)
{
	return files[i]->getName();
}

vector<string> AbstractDisk::getParentFileNames()
{
	vector<string> res;
	for (auto& f : parentFiles) {
		res.push_back(f->getName().length() < 8 ? f->getName() : f->getName().substr(0, 8));
	}
	return res;
}

bool AbstractDisk::renameSelectedFile(string s)
{
	auto directoryScreen = dynamic_pointer_cast<DirectoryScreen>(mpc.screens->getScreenComponent("directory"));
	auto loadScreen = dynamic_pointer_cast<LoadScreen>(mpc.screens->getScreenComponent("load"));
  
	auto left = directoryScreen->xPos == 0;
	auto fileNumber = left ? directoryScreen->yPos0 + directoryScreen->yOffset0 : loadScreen->fileLoad;

    auto file = left ? getParentFile(fileNumber) : getFile(fileNumber);
    return file->setName(s);
}

bool AbstractDisk::deleteSelectedFile()
{
	auto loadScreen = dynamic_pointer_cast<LoadScreen>(mpc.screens->getScreenComponent("load"));
	return files[loadScreen->fileLoad]->del();
}

std::vector<MpcFile*> AbstractDisk::getFiles()
{
	return files;
}

std::vector<MpcFile*> AbstractDisk::getAllFiles() {
	return allFiles;
}

MpcFile* AbstractDisk::getParentFile(int i)
{
	return parentFiles[i];
}

std::vector<MpcFile*> AbstractDisk::getParentFiles()
{
	return parentFiles;
}

void AbstractDisk::writeSound(std::weak_ptr<mpc::sampler::Sound> s)
{
	string name = mpc::Util::getFileName(s.lock()->getName() + ".SND");
    auto nf = newFile(name);
    writeSound(s.lock().get(), nf);
}

void AbstractDisk::writeWav(std::weak_ptr<mpc::sampler::Sound> s)
{
	string name = mpc::Util::getFileName(s.lock()->getName() + ".WAV");
	auto nf = newFile(name);
	writeWav(s.lock().get(), nf);
}

bool AbstractDisk::isDirectory(MpcFile* f)
{
	return f->isDirectory();
}

void AbstractDisk::writeSound(mpc::sampler::Sound* s, MpcFile* f)
{
	auto sw = mpc::file::sndwriter::SndWriter(s);
	auto sndArray = sw.getSndFileArray();
	f->setFileData(&sndArray);
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


void AbstractDisk::writeWav(mpc::sampler::Sound* s, MpcFile* f)
{
	auto data = s->getSampleData();
	
	auto wavFile = mpc::file::wav::WavFile::newWavFile(f->getFile().lock()->getPath(), s->isMono() ? 1 : 2, data->size() / (s->isMono() ? 1 : 2), 16, s->getSampleRate());

	if (!s->isMono()) {
		vector<float> interleaved;
		for (int i = 0; i < (int) (data->size() * 0.5); i++) {
			interleaved.push_back((*data)[i]);
			interleaved.push_back((*data)[(int) (i + data->size() * 0.5)]);
		}
		wavFile.writeFrames(&interleaved, data->size() / (s->isMono() ? 1 : 2));
	}
	else {
		wavFile.writeFrames(data, data->size() / (s->isMono() ? 1 : 2));
	}
	wavFile.close();
	flush();
	initFiles();
}

void AbstractDisk::writeSequence(mpc::sequencer::Sequence* s, string fileName)
{
	if (checkExists(fileName)) {
		return;
	}
	
	auto newMidFile = newFile(fileName);
	
	auto writer = mpc::file::mid::MidiWriter(s);
	writer.writeToFile(newMidFile->getFile().lock()->getPath());

	flush();
	initFiles();
}

bool AbstractDisk::checkExists(string fileName)
{
	initFiles();
	for (auto& str : getFileNames()) {
		if (StrUtil::eqIgnoreCase(str, fileName)) {
			return true;
		}
	}
	return false;
}

MpcFile* AbstractDisk::getFile(string fileName)
{
	auto tempfileName = StrUtil::replaceAll(fileName, ' ', "");
	for (auto& f : files) {
		if (StrUtil::eqIgnoreCase(StrUtil::replaceAll(f->getName(), ' ', ""), tempfileName)) {
			return f;
		}
	}
	for (auto& f : allFiles) {
		if (StrUtil::eqIgnoreCase(StrUtil::replaceAll(f->getName(), ' ', ""), tempfileName)) {
			return f;
		}
	}
	return nullptr;
}

void AbstractDisk::writeProgram(mpc::sampler::Program* program, string fileName)
{
	if (checkExists(fileName)) {
		return;
	}

	auto writer = mpc::file::pgmwriter::PgmWriter(program, Mpc::instance().getSampler());
	auto pgmFile = newFile(fileName);
    auto bytes = writer.get();
	pgmFile->setFileData(&bytes);
	vector<std::weak_ptr<mpc::sampler::Sound>> sounds;

	for (auto& n : program->getNotesParameters())
	{
		if (n->getSndNumber() != -1)
		{
			sounds.push_back(dynamic_pointer_cast<mpc::sampler::Sound>(Mpc::instance().getSampler().lock()->getSound(n->getSndNumber()).lock()));
		}
	}

	auto saveAProgramScreen = dynamic_pointer_cast<SaveAProgramScreen>(mpc.screens->getScreenComponent("save-a-program"));
	
	if (saveAProgramScreen->save != 0)
	{
		auto isWav = saveAProgramScreen->save == 1;
		soundSaver = make_unique<SoundSaver>(mpc, sounds, isWav);
	}
	else {
		mpc::Mpc::instance().getLayeredScreen().lock()->openScreen("save");
	}

	flush();
	initFiles();
	setBusy(false);
}

bool AbstractDisk::isRoot()
{
	return getPathDepth() == 0;
}

AbstractDisk::~AbstractDisk() {
	for (auto& f : parentFiles) {
		if (f != nullptr)
			delete f;
	}
	for (auto& f : allFiles) {
		if (f != nullptr)
			delete f;
	}
}
