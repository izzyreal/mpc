#include <disk/SoundLoader.hpp>

#include <Mpc.hpp>
#include <disk/MpcFile.hpp>
#include <file/sndreader/SndReader.hpp>
#include <file/wav/WavFile.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

#include <lcdgui/screens/dialog2/PopupScreen.hpp>

#include <lang/StrUtil.hpp>
#include <file/File.hpp>

#include <cmath>

using namespace mpc::disk;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::dialog2;
using namespace moduru::lang;
using namespace std;

SoundLoader::SoundLoader(mpc::Mpc& mpc)
	: mpc(mpc)
{
}
float SoundLoader::rateToTuneBase = (float)(pow(2, (1.0 / 12.0)));

SoundLoader::SoundLoader(mpc::Mpc& mpc, vector<weak_ptr<mpc::sampler::Sound>> sounds, bool replace)
	: mpc(mpc)
{
	
	this->replace = replace;
	this->sounds = sounds;
}

void SoundLoader::setPartOfProgram(bool b)
{
	partOfProgram = b;
}

int SoundLoader::loadSound(weak_ptr<MpcFile> f)
{
	auto sound = mpc.getSampler().lock()->addSound().lock();

	auto soundFile = f.lock();
	string soundFileName = soundFile->getName();
	auto periodIndex = soundFileName.find_last_of('.');
	string extension = "";
	string soundName = "";
	auto mono = false;
	auto sampleRate = 44100;
	auto start = 0;
	auto end = 0;
	auto loopTo = 0;
	auto level = 100;
	auto tune = 0;
	auto loopEnabled = false;
	auto beats = 4;
	
	if (periodIndex != string::npos)
	{
		extension = soundFileName.substr(periodIndex + 1, soundFileName.length());
		soundFileName = soundFileName.substr(0, periodIndex);
		soundName = soundFileName;
	}
	
	auto sampler = mpc.getSampler().lock();
	auto existingSoundIndex = sampler->checkExists(soundName);
	
	if (!partOfProgram && existingSoundIndex == -1)
	{
		mpc.getLayeredScreen().lock()->openScreen("popup");
		auto popupScreen = mpc.screens->get<PopupScreen>("popup");
		popupScreen->setText("LOADING " + StrUtil::padRight(soundFileName, " ", 16) + "." + extension);
	}

	std::vector<float>* fa = sound->getSampleData();

	if (StrUtil::eqIgnoreCase(extension, "wav"))
	{

		auto file = soundFile->getFile().lock();
		auto wavFile = mpc::file::wav::WavFile::openWavFile(file->getPath());
		
		if (wavFile.getValidBits() != 16)
		{
			wavFile.close();
			throw invalid_argument("Only 16 bit supported");
		}
		
		int numChannels = wavFile.getNumChannels();
		
		if (numChannels == 1)
		{
			wavFile.readFrames(fa, wavFile.getNumFrames());
		}
		else
		{
			fa->clear();
			vector<float> interleaved;
			wavFile.readFrames(&interleaved, wavFile.getNumFrames());
		
			for (int i = 0; i < interleaved.size(); i += 2)
				fa->push_back(interleaved[i]);
			
			for (int i = 1; i < interleaved.size(); i += 2)
				fa->push_back(interleaved[i]);
		}

		size = fa->size();
		end = size;
		
		if (numChannels == 1)
			mono = true;
		else
			end /= 2;
		
		sampleRate = wavFile.getSampleRate();
		
		loopTo = end;

		if (wavFile.getNumSampleLoops() > 0)
		{
			auto& sampleLoop = wavFile.getSampleLoop();
			loopTo = sampleLoop.start;
			end = sampleLoop.end;
			loopEnabled = true;
		}

		float tuneFactor = (float)(sampleRate / 44100.0);
		
		tune = (int)(floor(logOfBase(tuneFactor, rateToTuneBase) * 10.0));
		
		if (tune < -120)
			tune = -120;
		else if (tune > 120)
			tune = 120;
	}
	else if (StrUtil::eqIgnoreCase(extension, "snd"))
	{
		auto sndReader = mpc::file::sndreader::SndReader(soundFile.get());
		sndReader.getSampleData(fa);
		size = fa->size();
		mono = sndReader.isMono();
		start = sndReader.getStart();
		end = sndReader.getEnd();
		loopTo = end - sndReader.getLoopLength();
		sampleRate = sndReader.getSampleRate();
		soundName = sndReader.getName();
		loopEnabled = sndReader.isLoopEnabled();
		level = sndReader.getLevel();
		tune = sndReader.getTune();
		beats = sndReader.getNumberOfBeats();
	}

	sound->setSampleRate(sampleRate);
	sound->setName(soundName);
	sound->setMono(mono);
	sound->setStart(start);
	sound->setEnd(end);
	sound->setLoopTo(loopTo);
	sound->setLoopEnabled(loopEnabled);
	sound->setLevel(level);
	sound->setTune(tune);
	sound->setBeatCount(beats);

	bool alreadyLoaded = existingSoundIndex != -1;

	if (preview)
	{
		return existingSoundIndex;
	}
	else
	{
		if (existingSoundIndex == -1)
		{
			if (partOfProgram)
				return (int)(sampler->getSoundCount()) - 1;
		}
		else
		{
			if (replace)
			{
				sampler->deleteSound(existingSoundIndex);
				sound->setMemoryIndex(existingSoundIndex);
				sampler->sort();
			}
			else
			{
				sampler->deleteSound((int)(sampler->getSoundCount()) - 1);
			}
			
			if (partOfProgram)
				return existingSoundIndex;
		}
	}
	return -1;
}

void SoundLoader::getSampleDataFromWav(weak_ptr<moduru::file::File> soundFile, vector<float>* dest)
{
	auto wavFile = mpc::file::wav::WavFile::openWavFile(soundFile.lock()->getPath());
	wavFile.readFrames(dest, wavFile.getNumFrames());
}

void SoundLoader::setPreview(bool b)
{
	preview = b;
}

double SoundLoader::logOfBase(float num, float base)
{
	return log(num) / log(base);
}

int SoundLoader::getSize()
{
	return size;
}
