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
using namespace mpc::file::wav;

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

using namespace mpc::sampler;

using namespace moduru::lang;
using namespace moduru::file;

AbstractDisk::AbstractDisk(mpc::Mpc& _mpc)
	: mpc (_mpc)
{
}

std::shared_ptr<MpcFile> AbstractDisk::getFile(int i)
{
	return files[i];
}

std::vector<std::string> AbstractDisk::getFileNames()
{
    std::vector<std::string> res;
	transform(files.begin(), files.end(), back_inserter(res), [](std::shared_ptr<MpcFile> f) { return f->getName(); });
	return res;
}

std::string AbstractDisk::getFileName(int i)
{
	return files[i]->getName();
}

std::vector<std::string> AbstractDisk::getParentFileNames()
{
    std::vector<std::string> res;

	for (auto& f : parentFiles)
		res.push_back(f->getName().length() < 8 ? f->getName() : f->getName().substr(0, 8));

	return res;
}

bool AbstractDisk::deleteSelectedFile()
{
	auto loadScreen = mpc.screens->get<LoadScreen>("load");
	return files[loadScreen->fileLoad]->del();
}

std::vector<std::shared_ptr<MpcFile>>& AbstractDisk::getAllFiles() {
	return allFiles;
}

std::shared_ptr<MpcFile> AbstractDisk::getParentFile(int i)
{
    return parentFiles[i];
}

void AbstractDisk::writeSound(std::weak_ptr<Sound> s)
{
	auto name = mpc::Util::getFileName(s.lock()->getName() + ".SND");
    writeSound(s, newFile(name));
}

void AbstractDisk::writeWav(std::weak_ptr<Sound> s)
{
	auto name = mpc::Util::getFileName(s.lock()->getName() + ".WAV");
	writeWav(s, newFile(name));
}

void AbstractDisk::writeSound(std::weak_ptr<Sound> s, std::weak_ptr<MpcFile> f)
{
	auto sw = mpc::file::sndwriter::SndWriter(s.lock().get());
	auto sndArray = sw.getSndFileArray();
	f.lock()->setFileData(sndArray);
	flush();
	initFiles();
}

void AbstractDisk::writeWav(std::weak_ptr<Sound> s, std::weak_ptr<MpcFile> f)
{
    auto sound = s.lock();
    auto data = sound->getSampleData();
    auto isMono = sound->isMono();

    auto outputStream = f.lock()->getOutputStream();

    auto wavFile = WavFile::writeWavStream(outputStream, isMono ? 1 : 2, data->size() / (isMono ? 1 : 2), 16, sound->getSampleRate());

    if (isMono)
    {
        wavFile.writeFrames(data, data->size());
    }
    else
    {
        std::vector<float> interleaved;

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

void AbstractDisk::writeSequence(std::weak_ptr<mpc::sequencer::Sequence> s, std::string fileName)
{
	if (checkExists(fileName))
		return;

	auto newMidFile = newFile(fileName);

	auto writer = mpc::file::mid::MidiWriter(s.lock().get());
	writer.writeToOStream(newMidFile->getOutputStream());

	flush();
	initFiles();
}

bool AbstractDisk::checkExists(std::string fileName)
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

std::shared_ptr<MpcFile> AbstractDisk::getFile(const std::string& fileName)
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

void AbstractDisk::writeProgram(std::weak_ptr<Program> program, const std::string& fileName)
{
	if (checkExists(fileName))
		return;

	auto writer = mpc::file::pgmwriter::PgmWriter(program.lock().get(), mpc.getSampler());
	auto pgmFile = newFile(fileName);
    auto bytes = writer.get();
	pgmFile->setFileData(bytes);
    
    std::vector<std::weak_ptr<Sound>> sounds;

	for (auto& n : program.lock()->getNotesParameters())
	{
		if (n->getSoundIndex() != -1)
            sounds.push_back(mpc.getSampler().lock()->getSound(n->getSoundIndex()).lock());
	}

	auto saveAProgramScreen = mpc.screens->get<SaveAProgramScreen>("save-a-program");

	if (saveAProgramScreen->save != 0)
	{
		auto isWav = saveAProgramScreen->save == 2;
		soundSaver = std::make_unique<SoundSaver>(mpc, sounds, isWav);
	}
	else
    {
		mpc.getLayeredScreen().lock()->openScreen("save");
	}

	flush();
	initFiles();
}

bool AbstractDisk::isRoot()
{
	return getPathDepth() == 0;
}

bool AbstractDisk::deleteRecursive(std::weak_ptr<MpcFile> _toDelete)
{
    auto toDelete = _toDelete.lock();
    
    if (toDelete->isDirectory())
    {
        for (auto& f : toDelete->listFiles())
        {
            if (f->getName() == "" || f->getName() == "." || f->getName() == "..")
                continue;
            
            deleteRecursive(f);
        }
    }
    
    return toDelete->del();
}

