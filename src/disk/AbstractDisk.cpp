#include <disk/AbstractDisk.hpp>

#include <Mpc.hpp>
#include <Util.hpp>
#include <disk/MpcFile.hpp>
#include <file/wav/WavFile.hpp>
#include <disk/SoundSaver.hpp>
#include <file/mid/MidiWriter.hpp>
#include <file/pgmwriter/PgmWriter.hpp>
#include <file/sndwriter/SndWriter.hpp>
#include <ui/Uis.hpp>
#include <ui/disk/DiskGui.hpp>
#include <ui/disk/window/DirectoryGui.hpp>
#include <sampler/NoteParameters.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::disk;
using namespace moduru::lang;
using namespace std;

AbstractDisk::AbstractDisk(weak_ptr<mpc::disk::Store> store, mpc::Mpc* mpc)
{
	this->store = store;
	this->mpc = mpc;
	this->diskGui = mpc->getUis().lock()->getDiskGui();
	extensions = vector<string>{ "", "SND", "PGM", "APS", "MID", "ALL", "WAV", "SEQ", "SET" };
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
	if(t > 1) {
		hrSize = StrUtil::TrimDecimals(t, 2) + " TB";
	} else if(g > 1) {
		hrSize = StrUtil::TrimDecimals(g, 2) + " GB";
	} else if(m > 1) {
		hrSize = StrUtil::TrimDecimals(m, 2) + " MB";
	} else if(k > 1) {
		hrSize = StrUtil::TrimDecimals(k, 2) + " KB";
	} else {
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
	for (auto& f : files) {
		res.push_back(f->getName());
	}
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
    auto dirGui = mpc->getUis().lock()->getDirectoryGui();
	auto diskGui = mpc->getUis().lock()->getDiskGui();
    auto left = dirGui->getXPos() == 0;
    auto fileNumber = left ? dirGui->getYpos0() + dirGui->getYOffsetFirst() : diskGui->getFileLoad();
    auto file = left ? getParentFile(fileNumber) : getFile(fileNumber);
    return file->setName(s);
}

bool AbstractDisk::deleteSelectedFile()
{
	auto diskGui = mpc->getUis().lock()->getDiskGui();
	return files[diskGui->getFileLoad()]->del();
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
	auto sw = new mpc::file::sndwriter::SndWriter(s);
	auto sndArray = sw->getSndFileArray();
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
	mpc::file::wav::WavFile wavFile;
	wavFile.newWavFile(s->isMono() ? 1 : 2, data->size() / (s->isMono() ? 1 : 2), 16, s->getSampleRate());
	if (!s->isMono()) {
		vector<float> interleaved;
		for (int i = 0; i < data->size() / 2; i++) {
			interleaved.push_back((*data)[i]);
			interleaved.push_back((*data)[i + data->size() / 2]);
		}
		wavFile.writeFrames(&interleaved, data->size() / (s->isMono() ? 1 : 2));
	}
	else {
		wavFile.writeFrames(data, data->size() / (s->isMono() ? 1 : 2));
	}
	wavFile.close();
	auto wavBytes = wavFile.getResult();
	f->setFileData(&wavBytes);
	flush();
	initFiles();
}

void AbstractDisk::writeWavToTemp(mpc::sampler::Sound* s)
{
	/*
    auto f = new File(("user.home"))->append("/Mpc/temp/")
        ->append(s->getName())
        ->append(".WAV")->toString());
    auto fa = s->getSampleData();
    auto faDouble = new ::doubleArray(fa->length);
    for (int i = 0; i < faDouble.size(); i++)
                (*faDouble)[i] = (*fa)[i];

    try {
        auto fos = new ::java::io::FileOutputStream(f->getPath());
        auto wavFile = mpc::file::wav::WavFile::newWavFile(s->isMono() ? 1 : 2, faDouble.size() / (s->isMono() ? 1 : 2), 16, s->getSampleRate());
        wavFile->writeFrames(faDouble, faDouble.size() / (s->isMono() ? 1 : 2));
        wavFile->close();
        auto wavBytes = wavFile->getResult();
        fos->write(wavBytes);
        fos->close();
    } catch (::java::lang::Exception* e) {
        e->printStackTrace();
    }
	*/
	flush();
    initFiles();
}

void AbstractDisk::writeSequence(mpc::sequencer::Sequence* s, string fileName)
{
	if (checkExists(fileName)) return;
	auto mw = mpc::file::mid::MidiWriter(s);
	auto mfArray_ = mw.getBytes();
	auto newMidFile = newFile(fileName);
	newMidFile->setFileData(&mfArray_);
	flush();
	initFiles();
}

bool AbstractDisk::checkExists(string fileName)
{
	for (auto& str : getFileNames()) {
		{
			if (StrUtil::eqIgnoreCase(str, fileName)) {
				return true;
			}
		}
	}
	return false;
}

MpcFile* AbstractDisk::getFile(string fileName)
{
	auto tempfileName = StrUtil::replaceAll(fileName, ' ', "");
	for (auto& f : files) {
		if (StrUtil::eqIgnoreCase(StrUtil::replaceAll(f->getName(), ' ', ""), tempfileName))
			return f;
	}
	for (auto& f : allFiles) {
		if (StrUtil::eqIgnoreCase(StrUtil::replaceAll(f->getName(), ' ', ""), tempfileName))
			return f;
	}
	return nullptr;
}

void AbstractDisk::writeProgram(mpc::sampler::Program* program, string fileName)
{
	if (checkExists(fileName)) return;
	auto writer = mpc::file::pgmwriter::PgmWriter(program, mpc->getSampler());
	auto pgmFile = newFile(fileName);
    auto bytes = writer.get();
	pgmFile->setFileData(&bytes);
	vector<std::weak_ptr<mpc::sampler::Sound> > sounds;
	for (auto& n : program->getNotesParameters()) {
		if (n->getSndNumber() != -1) {
			sounds.push_back(mpc->getSampler().lock()->getSound(n->getSndNumber()));
		}
	}
	auto save = mpc->getUis().lock()->getDiskGui()->getPgmSave();
	if (save != 0) {
		SoundSaver(mpc, sounds, save == 1 ? false : true);
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
