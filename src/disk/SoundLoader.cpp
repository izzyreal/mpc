#include <disk/SoundLoader.hpp>

#include <Mpc.hpp>
#include <disk/MpcFile.hpp>
#include <file/sndreader/SndReader.hpp>
#include <file/wav/WavFile.hpp>
#include <ui/Uis.hpp>
#include <ui/disk/DiskGui.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

#include <lang/StrUtil.hpp>

#include <cmath>

using namespace moduru::lang;
using namespace mpc::disk;
using namespace std;

SoundLoader::SoundLoader(mpc::Mpc* mpc)
{
	this->mpc = mpc;
}
float SoundLoader::rateToTuneBase = (float)(pow(2, (1.0 / 12.0)));

SoundLoader::SoundLoader(mpc::Mpc* mpc, vector<weak_ptr<mpc::sampler::Sound>> sounds, bool replace)
{
	this->mpc = mpc;
	this->replace = replace;
	this->sounds = sounds;
}

void SoundLoader::setPartOfProgram(bool b)
{
	partOfProgram = b;
}

int SoundLoader::loadSound(MpcFile* f)
{
	soundFile = f;
	string soundFileName = soundFile->getName();
	auto periodIndex = soundFileName.find_last_of('.');
	string extension = "";
	string soundName = "";
	auto mono = false;
	auto sampleRate = 0;
	auto start = 0;
	auto end = 0;
	auto loopTo = 0;
	auto level = 100;
	auto tune = 0;
	auto loopEnabled = false;
	auto beats = 4;
	if (periodIndex != string::npos) {
		extension = soundFileName.substr(periodIndex + 1, soundFileName.length());
		soundFileName = soundFileName.substr(0, periodIndex);
		soundName = soundFileName;
	}
	
	auto sampler = mpc->getSampler().lock();
	auto existIndex = sampler->checkExists(soundName);
	if (!partOfProgram && existIndex == -1) {
		mpc->getUis().lock()->getDiskGui()->openPopup(soundFileName, extension);
	}

	auto sound = sampler->addSound(sampleRate).lock();
	std::vector<float>* fa = sound->getSampleData();
	if (StrUtil::eqIgnoreCase(extension, "wav")) {
		auto file = soundFile->getFile().lock();
		mpc::file::wav::WavFile wavFile;
		wavFile.openWavFile(file);

		int numChannels = wavFile.getNumChannels();
		if (numChannels == 1) {
			wavFile.readFrames(fa, wavFile.getNumFrames());
		}
		else {
			fa->clear();
			vector<float> interleaved;
			wavFile.readFrames(&interleaved, wavFile.getNumFrames());
			for (int i = 0; i < interleaved.size(); i += 2) {
				fa->push_back(interleaved[i]);
			}
			for (int i = 1; i < interleaved.size(); i += 2) {
				fa->push_back(interleaved[i]);
			}
		}
		size = fa->size();
		end = size;
		if (numChannels == 1) {
			mono = true;
		}
		else {
			end /= 2;
		}
		sampleRate = wavFile.getSampleRate();
		loopTo = end;
		float tuneFactor = (float)(sampleRate / 44100.0);
		tune = (int)(floor(logOfBase(tuneFactor, rateToTuneBase) * 10.0));
		if (tune < -120)
			tune = -120;

		if (tune > 120)
			tune = 120;

	}
	else if (StrUtil::eqIgnoreCase(extension, "snd")) {
		auto sndReader = mpc::file::sndreader::SndReader(soundFile);
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
	sound->setName(soundName);
	sound->setMono(mono);
	sound->setStart(start);
	sound->setEnd(end);
	sound->setLoopTo(loopTo);
	sound->setLoopEnabled(loopEnabled);
	sound->setLevel(level);
	sound->setTune(tune);
	sound->setNumberOfBeats(beats);
	if (!preview) {
		if (existIndex == -1) {
			if (partOfProgram) return (int)(sampler->getSoundCount()) - 1;
		}
		else {
			if (replace) {
				sound->setMemoryIndex(existIndex);
				sampler->deleteSample(existIndex);
				sampler->sort();
			}
			else {
				sampler->deleteSample((int)(sampler->getSoundCount()) - 1);
			}
			if (partOfProgram)
				return existIndex;
		}
	}
	else {
		return existIndex;
	}
	return -1;
}

std::vector<float> SoundLoader::getSampleDataFromWav(weak_ptr<moduru::file::File> soundFile)
{
	mpc::file::wav::WavFile wavFile;
	wavFile.openWavFile(soundFile);
	std::vector<float> floatData;
	wavFile.readFrames(&floatData, wavFile.getNumFrames());
	return floatData;
}

void SoundLoader::getSampleDataFromWav(weak_ptr<moduru::file::File> soundFile, vector<float>* dest)
{
	mpc::file::wav::WavFile wavFile;
	wavFile.openWavFile(soundFile);
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
