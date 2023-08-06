#include "Sampler.hpp"

#include <Mpc.hpp>
#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/WavInputStringStream.hpp>

#include <sampler/NoteParameters.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sound.hpp>
#include <sequencer/Track.hpp>

#include <lcdgui/screens/ZoneScreen.hpp>
#include <lcdgui/screens/dialog/MetronomeSoundScreen.hpp>

#include <engine/audio/server/NonRealTimeAudioServer.hpp>

#include "engine/PreviewSoundPlayer.hpp"
#include "engine/StereoMixer.hpp"
#include "engine/IndivFxMixer.hpp"

#include <StrUtil.hpp>

#include <samplerate.h>

#include <cmrc/cmrc.hpp>
#include <string_view>

CMRC_DECLARE(mpc);

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::sampler;
using namespace mpc::engine;

Sampler::Sampler(mpc::Mpc& mpc)
	: mpc(mpc)
{
}

std::shared_ptr<Sound> Sampler::getPreviewSound()
{
    return sounds.back();
}

std::shared_ptr<Sound> Sampler::getSound(int index)
{
    if (index < 0 || index >= sounds.size())
        return {};

    return sounds[index];
}

void Sampler::replaceSound(int index, std::shared_ptr<Sound>& newSound)
{
    int newIndex = -1;

    for (size_t i = 0; i < sounds.size(); i++)
    {
        if (sounds[i] == newSound)
        {
            newIndex = i;
            break;
        }
    }

    if (newIndex != -1)
    {
        sounds[index] = sounds[newIndex];
        sounds.erase(sounds.begin() + newIndex);
    }
}

std::shared_ptr<Program> Sampler::getProgram(int index)
{
    return programs[index];
}

void Sampler::nudgeSoundIndex(bool up)
{
    if (up)
    {
        if (soundIndex >= sounds.size() - 1)
        {
            return;
        }

        soundIndex++;
        printf("");
        return;
    }

    if (soundIndex == 0)
    {
        return;
    }

    soundIndex--;
}

void Sampler::setSoundIndex(int i)
{
	if (i < 0 || i >= sounds.size())
    {
        return;
    }

	soundIndex = i;

	auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");
	zoneScreen->initZones();
}

short Sampler::getSoundIndex()
{
	return soundIndex;
}

std::shared_ptr<Sound> Sampler::getSound()
{
	if (soundIndex < 0)
	{
		if (sounds.empty())
			return {};

		// This is a bit of a hack, depending on what the real 2KXL does.
		// For now this will work fine to get some kind of sane index here after loading an APS, PGM, SND or WAV into an empty sampler.
		soundIndex = 0;
	}

	if (soundIndex >= sounds.size())
		return {};

	return getSortedSounds()[soundIndex].first;
}

std::string Sampler::getPreviousScreenName()
{
	return previousScreenName;
}

void Sampler::setPreviousScreenName(std::string s)
{
	previousScreenName = s;
}

std::vector<std::shared_ptr<StereoMixer>>& Sampler::getDrumStereoMixerChannels(int i)
{
	return mpc.getDrum(i).getStereoMixerChannels();
}

std::vector<std::shared_ptr<IndivFxMixer>>& Sampler::getDrumIndivFxMixerChannels(int i)
{
	return mpc.getDrum(i).getIndivFxMixerChannels();
}

std::vector<int>* Sampler::getInitMasterPadAssign()
{
	return &initMasterPadAssign;
}

std::vector<int>* Sampler::getMasterPadAssign()
{
	return &masterPadAssign;
}

void Sampler::setMasterPadAssign(std::vector<int> v)
{
	masterPadAssign = v;
}

void Sampler::init()
{
    initMasterPadAssign = Pad::getPadNotes(mpc);

	auto program = createNewProgramAddFirstAvailableSlot().lock();
	program->setName("NewPgm-A");
	
	for (int i = 0; i < 4; i++)
	{
		for (auto j = 0; j < 16; j++)
		{
			std::string result = "";
			result.append(abcd[i]);
			result.append(StrUtil::padLeft(std::to_string(j + 1), "0", 2));
			padNames.push_back(result);
		}
	}

    auto fs = cmrc::mpc::get_filesystem();
    clickSound = std::make_shared<Sound>(44100);
    clickSound->setMono(true);
    clickSound->setLevel(100);

    if (fs.exists("audio/click.wav"))
    {
        auto clickFile = fs.open("audio/click.wav");
        auto clickData = (char*) std::string_view(clickFile.begin(), clickFile.end() - clickFile.begin()).data();

        auto stream = wav_init_istringstream(clickData, clickFile.size());
        int sampleRate, validBits, numChannels, numFrames;
        wav_read_header(stream, sampleRate, validBits, numChannels, numFrames);
        
        if (numChannels == 1 && validBits == 16) {
            for (int i = 0; i < numFrames; i++)
            {
                float frame = wav_get_LE(stream, 2) / 32768.0;
                clickSound->insertFrame(std::vector<float>{frame}, clickSound->getFrameCount());
            }
        }
        clickSound->setEnd(numFrames);
    }
    
	masterPadAssign = initMasterPadAssign;
}

void Sampler::playMetronome(unsigned int velocity, int framePos)
{
	auto metronomeSoundScreen = mpc.screens->get<MetronomeSoundScreen>("metronome-sound");
	auto soundNumber = -2;

	if (metronomeSoundScreen->getSound() != 0)
	{
		auto program = mpc.getDrum(metronomeSoundScreen->getSound() - 1).getProgram();
		auto accent = velocity == 127;
		velocity = accent ? metronomeSoundScreen->getAccentVelo() : metronomeSoundScreen->getNormalVelo();
		auto pad = accent ? metronomeSoundScreen->getAccentPad() : metronomeSoundScreen->getNormalPad();
		auto note = programs[program]->getNoteFromPad(pad);
		soundNumber = programs[program]->getNoteParameters(note)->getSoundIndex();
	}
	else
	{
		velocity *= metronomeSoundScreen->getVolume() * 0.01;
	}
	
	mpc.getBasicPlayer().mpcNoteOn(soundNumber, velocity, framePos);
}

void Sampler::playPreviewSample(int start, int end, int loopTo)
{
	if (sounds.empty())
	{
		return;
	}

	auto previewSound = sounds[sounds.size() - 1];
	auto oldStart = previewSound->getStart();
	auto oldEnd = previewSound->getEnd();
	auto oldLoopTo = previewSound->getLoopTo();
	previewSound->setStart(start);
	previewSound->setEnd(end);
	previewSound->setLoopTo(loopTo);
	mpc.getBasicPlayer().mpcNoteOn(-3, 127, 0);
	previewSound->setStart(oldStart);
	previewSound->setEnd(oldEnd);
	previewSound->setLoopTo(oldLoopTo);
}

int Sampler::getProgramCount()
{
	int res = 0;

	for (auto& p : programs)
	{
		if (p)
			res++;
	}

	return res;
}

std::weak_ptr<Program> Sampler::addProgram(int i)
{
	programs[i] = std::make_shared<Program>(mpc, this);
	return programs[i];
}

std::weak_ptr<Program> Sampler::createNewProgramAddFirstAvailableSlot()
{
    const bool repairDrumPrograms = getProgramCount() == 0;

 	for (auto& p : programs)
	{
		if (!p)
		{
			p = std::make_shared<Program>(mpc, this);

            if (repairDrumPrograms)
            {
                for (int i = 0; i < 4; i++)
                {
                    setDrumBusProgramIndex(i + 1, 0);
                }
            }
			return p;
		}
	}

	return std::weak_ptr<Program>();
}

void Sampler::deleteProgram(std::weak_ptr<Program> _program)
{
	for (auto&& p : programs)
	{
		if (p == _program.lock())
		{
			p.reset();
			break;
		}
	}

    repairProgramReferences();
}

std::vector<std::shared_ptr<Sound>>& Sampler::getSounds()
{
	return sounds;
}

std::shared_ptr<Sound> Sampler::addSound()
{
	return addSound(44100);
}

std::shared_ptr<Sound> Sampler::addSound(int sampleRate)
{
	auto res = std::make_shared<Sound>(sampleRate);
	sounds.push_back(res);
	return res;
}

int Sampler::getSoundCount()
{
	return sounds.size();
}

std::string Sampler::getSoundName(int i)
{
	return sounds[i]->getName();
}


std::string Sampler::getPadName(int i)
{
    if (i < 0)
    {
        return "OFF";
    }
	return padNames[i];
}

std::vector<std::weak_ptr<Program>> Sampler::getPrograms()
{
	auto res = std::vector<std::weak_ptr<Program>>();

	for (auto& p : programs)
		res.push_back(p);

	return res;
}

void Sampler::deleteAllPrograms(bool createDefaultProgram)
{
	for (auto& p : programs)
    {
        p.reset();
    }

    if (createDefaultProgram)
    {
        createNewProgramAddFirstAvailableSlot().lock()->setName("NewPgm-A");
    }
}

void Sampler::repairProgramReferences()
{
	for (int busIndex = 1; busIndex < 5; busIndex++)
	{
		auto pgm = getDrumBusProgramIndex(busIndex);
		
		if (!programs[pgm])
		{
            for (int programIndex = pgm - 1; programIndex > 0; programIndex--) {
                if (programs[programIndex]) {
                    pgm = programIndex;
                    break;
                }
            }

            if (!programs[pgm]) {
                for (int programIndex = 0; programIndex < 24; programIndex++)
                {
                    if (programs[programIndex])
                    {
                        pgm = programIndex;
                        break;
                    }
                }
            }

            setDrumBusProgramIndex(busIndex, pgm);
		}
	}
}

void Sampler::trimSample(int sampleNumber, int start, int end)
{
	auto s = sounds[sampleNumber];
	trimSample(s, start, end);
}

void Sampler::trimSample(std::weak_ptr<Sound> sound, int start, int end)
{
	auto s = sound.lock();
	auto data = s->getSampleData();
	auto frameCount = s->getFrameCount();

	if (end > frameCount)
		end = frameCount;

	if (s->isMono())
	{
		data->erase(data->begin() + end, data->end());
		data->erase(data->begin(), data->begin() + start);
	}
	else
	{
		int startRight = start + frameCount;
		int endRight = end + frameCount;

		data->erase(data->begin() + endRight, data->end());
		data->erase(data->begin() + frameCount, data->begin() + startRight);
		data->erase(data->begin() + end, data->begin() + frameCount);
		data->erase(data->begin(), data->begin() + start);
	}

	s->setStart(0);
	s->setEnd(s->getFrameCount());
	s->setLoopTo(s->getFrameCount());
}

void Sampler::deleteSection(const unsigned int sampleNumber, const unsigned int start, const unsigned int end)
{
	auto s = sounds[sampleNumber];
	auto data = s->getSampleData();
	auto frameCount = s->getFrameCount();

	if (!s->isMono())
	{
		const unsigned int startRight = start + frameCount;
		const unsigned int endRight = end + frameCount;
		data->erase(data->begin() + startRight, data->begin() + endRight);
	}

	data->erase(data->begin() + start, data->begin() + end);
}

std::string Sampler::getSoundSortingTypeName()
{
	if (soundSortingType == 0)
		return "MEMORY";
	else if (soundSortingType == 1)
		return "NAME";
	else
		return "SIZE";
}

void Sampler::switchToNextSoundSortType()
{
    auto s = getSound();

    if (soundSortingType++ >= 2)
    {
        soundSortingType = 0;
    }

    auto sortedSounds = getSortedSounds();

    for (int i = 0; i < sortedSounds.size(); i++)
    {
        if (sortedSounds[i].first == s)
        {
            soundIndex = i;
            printf("");
            break;
        }
    }
}

void Sampler::deleteAllSamples()
{
	sounds.clear();
	
	for (auto& p : programs)
	{
		if (!p)
			continue;

		for (auto& n : p->getNotesParameters())
			n->setSoundIndex(-1);
	}

    soundIndex = 0;
}

void Sampler::process12Bit(std::vector<float>* fa)
{
	for (auto j = 0; j < fa->size(); j++)
	{
		if ((*fa)[j] != 0.0f)
		{
			auto fShort = static_cast< int16_t >((*fa)[j] * 32767.4999999);

			if ((*fa)[j] > 0.9999999f)
				fShort = 32767;

			int16_t newShort = fShort;
			newShort &= ~(1 << 0);
			newShort &= ~(1 << 1);
			newShort &= ~(1 << 2);
			newShort &= ~(1 << 3);

			(*fa)[j] = static_cast< float >(newShort / 32767.4999999);
		}
		else
		{
			(*fa)[j] = 0;
		}
	}
}

void Sampler::process8Bit(std::vector<float>* fa)
{
	for (auto j = 0; j < fa->size(); j++)
	{
		if ((*fa)[j] != 0.0f)
		{
			float f = (*fa)[j];
		
			if (f < -1)
				f = -1;
			else if (f > 1)
				f = 1;

			unsigned short ushort = (signed short)((f + 1)  * 32767.4999999);
			unsigned char eightBit = ushort >> 8;
			signed sshort = (eightBit - 128) << 8;
			
			f = (float)(sshort / 32767.49999999);
			
			if (f < -1)
				f = -1;
			else if (f > 1)
				f = 1;
			
			(*fa)[j] = f;
		}
	}
}

std::vector<float> Sampler::resampleSingleChannel(std::vector<float>& input, int sourceRate, int destRate)
{
    const auto ratio = (double) destRate / sourceRate;
    const auto outputFrameCount = static_cast<int>(ceil(input.size() * ratio));

    std::vector<float> result(outputFrameCount);

    SRC_DATA srcData;
    srcData.data_in = &input[0];
    srcData.input_frames = input.size();
    srcData.data_out = &result[0];
    srcData.output_frames = outputFrameCount;
    srcData.src_ratio = 1.0 / ratio;

    auto error = src_simple(&srcData, 0, 1);

    if (error != 0)
    {
        const char* errormsg = src_strerror(error);
        std::string errorStr(errormsg);
        MLOG("libsamplerate error: " + errorStr);
    }

    return result;
}

void Sampler::resample(std::vector<float>& data, int sourceRate, std::shared_ptr<Sound> destSnd)
{
    float* srcArray = &data[0];

    SRC_DATA srcData;
    srcData.data_in = srcArray;
    srcData.input_frames = data.size();
    srcData.src_ratio = (double)(destSnd->getSampleRate()) / (double)(sourceRate);
    
    srcData.output_frames = (floor)(data.size() * srcData.src_ratio);

    auto dest = destSnd->getSampleData();
    dest->resize(srcData.output_frames);

    float* destArray = &(*dest)[0];
    srcData.data_out = destArray;

    auto error = src_simple(&srcData, 0, 1);

    if (error != 0)
    {
        const char* errormsg = src_strerror(error);
        std::string errorStr(errormsg);
        MLOG("libsamplerate error: " + errorStr);
    }
}

std::weak_ptr<Sound> Sampler::createZone(std::weak_ptr<Sound> source, int start, int end, int endMargin)
{
	auto overlap = (int)(endMargin * source.lock()->getSampleRate() * 0.001);

	auto zone = copySound(source);
	auto zoneLength = end - start + overlap;
	auto endCandidate = start + zoneLength;
	trimSample(zone, start, endCandidate);
	return zone;
}

void Sampler::stopAllVoices(int frameOffset)
{
	mpc.getDrum(0).allSoundOff(frameOffset);
}

void Sampler::finishBasicVoice()
{
	mpc.getBasicPlayer().finishVoice();
}

void Sampler::playX()
{
    auto sound = getSortedSounds()[soundIndex].first;
	auto start = 0;
	auto end = sound->getSampleData()->size() - 1;

	if (!sound->isMono())
		end *= 0.5;

	auto fullEnd = end;

	if (playXMode == 1)
	{
		auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");
		auto zone = zoneScreen->getZone();
		start = zone[0];
		end = zone[1];
	}
	else if (playXMode == 2)
	{
		end = sound->getStart();
	}
	else if (playXMode == 3)
	{
		end = sound->getLoopTo();
	}
	else if (playXMode == 4)
	{
		start = sound->getEnd();
		end = fullEnd;
	}

	int oldStart = sound->getStart();
	int oldEnd = sound->getEnd();
	sound->setStart(start);
	sound->setEnd(end);
	mpc.getBasicPlayer().mpcNoteOn(-4, 127, 0);
	sound->setStart(oldStart);
	sound->setEnd(oldEnd);
}

std::shared_ptr<Sound> Sampler::getPlayXSound()
{
	return getSortedSounds()[soundIndex].first;
}

int Sampler::getFreeSampleSpace()
{
	double freeSpace = 32620;

	for (auto& s : sounds)
		freeSpace -= (s->getSampleData()->size() * 2) / (double) 1024;

	return (int) floor(freeSpace);
}

int Sampler::getLastInt(std::string s)
{
	auto offset = s.length();

	for (int i = s.length() - 1; i >= 0; i--)
	{
		auto c = s[i];

		if (isdigit(c))
		{
			offset--;
		}
		else
		{
			if (offset == s.length())
				return INT_MIN;

			break;
		}
	}
	return stoi(s.substr(offset));
}

std::string Sampler::addOrIncreaseNumber(std::string s)
{
	auto res = s;
	bool exists = true;
	
	while (exists)
	{
		exists = false;

		for (int i = 0; i < getSoundCount(); i++)
		{
			if (getSoundName(i) == res)
			{
				exists = true;
				res = addOrIncreaseNumber2(res);
				break;
			}
		}
	}

	return res;
}

std::string Sampler::addOrIncreaseNumber2(std::string s)
{
	int candidate = getLastInt(s);
	std::string res = s;
	
	if (candidate == INT_MIN)
	{
        if (res.length() >= 16) res = res.substr(0, 15);
        return res + std::to_string(1);
	}

    auto candidateStr = std::to_string(candidate);
    int candidateLength = candidateStr.length();

    res = res.substr(0, res.length() - candidateLength);

    candidate++;

    candidateStr = std::to_string(candidate);
    candidateLength = candidateStr.length();

    if (res.length() + candidateLength > 16)
        res = res.substr(0, 16 - candidateLength);

    res += candidateStr;
	return res;
}

Pad* Sampler::getLastPad(Program* program)
{
	return program->getPad(mpc.getPad());
}

NoteParameters* Sampler::getLastNp(Program* program)
{
	return dynamic_cast<mpc::sampler::NoteParameters*>(program->getNoteParameters(mpc.getNote()));
}

std::vector<std::shared_ptr<Sound>> Sampler::getUsedSounds()
{
	std::set<std::shared_ptr<Sound>> usedSounds;

	for (auto& p : programs)
	{
		if (!p)
			continue;
	
		for (auto& nn : p->getNotesParameters())
		{
			if (nn->getSoundIndex() != -1)
				usedSounds.insert(sounds[nn->getSoundIndex()]);
		}
	}
	return {begin(usedSounds), end(usedSounds)};
}

int Sampler::getUnusedSampleCount()
{
	return (int)(sounds.size() - getUsedSounds().size());
}

void Sampler::purge()
{
	auto usedSounds = getUsedSounds();

	for (int i = sounds.size() - 1; i >= 0; i--)
	{
        auto maybeUnusedSound = sounds[i];

		const auto pos = find_if(begin(usedSounds), end(usedSounds), [&maybeUnusedSound](const std::shared_ptr<Sound>& sound) {
			return sound == maybeUnusedSound;
		});
		
		if (pos == usedSounds.end())
		{
            deleteSound(maybeUnusedSound);
        }
	}
}


void Sampler::deleteSound(int deleteSoundIndex)
{
	deleteSound(sounds[deleteSoundIndex]);
}

void Sampler::deleteSoundWithoutRepairingPrograms(std::shared_ptr<Sound> sound)
{
    int index = -1;

    for (int i = 0; i < sounds.size(); i ++)
    {
        if (sounds[i] == sound)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        return;
    }

    sounds.erase(sounds.begin() + index);
}

void Sampler::deleteSound(const std::shared_ptr<Sound>& sound)
{
	int index = -1;

    for (int i = 0; i < sounds.size(); i ++)
    {
        if (sounds[i] == sound)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        return;
    }

    for (auto& p : programs)
    {
        if (!p) continue;

        for (auto& n : p->getNotesParameters())
        {
            if (n->getSoundIndex() == index)
            {
                n->setSoundIndex(-1);
            }
            else if (n->getSoundIndex() > index)
            {
                n->setSoundIndex(n->getSoundIndex() - 1);
            }
        }
    }

    sounds.erase(sounds.begin() + index);

	if (soundIndex >= sounds.size())
    {
        soundIndex--;
    }

	if (soundIndex < 0)
    {
        soundIndex = 0;
    }
}

std::vector<float> Sampler::mergeToStereo(std::vector<float> fa0, std::vector<float> fa1)
{
	const int newLengthFrames = fa0.size() > fa1.size() ? fa0.size() : fa1.size();
	std::vector<float> newSampleData = std::vector<float>(newLengthFrames * 2);

	for (int i = 0; i < newLengthFrames; i++)
	{
		if (i < fa0.size())
		{
			newSampleData[i] = fa0[i];
		}
		else
		{
			newSampleData[i] = 0;
		}
		
		if (i < fa1.size())
		{
			newSampleData[i + newLengthFrames] = fa1[i];
		}
		else {
			newSampleData[i + newLengthFrames] = 0;
		}
	}
	return newSampleData;
}

void Sampler::mergeToStereo(std::vector<float>* sourceLeft, std::vector<float>* sourceRight, std::vector<float>* dest)
{
	dest->clear();

	for (auto& f : *sourceLeft)
	{
		dest->push_back(f);
	}
	
	if (sourceLeft->size() < sourceRight->size())
	{
		auto diff = sourceRight->size() - sourceLeft->size();
	
		for (int i = 0; i < diff; i++)
		{
			dest->push_back(0);
		}
	}
	
	for (auto& f : *sourceRight)
	{
		dest->push_back(f);
	}
	
	if (sourceRight->size() < sourceLeft->size())
	{
		auto diff = sourceLeft->size() - sourceRight->size();
		for (int i = 0; i < diff; i++)
		{
			dest->push_back(0);
		}
	}
}

void Sampler::setDrumBusProgramIndex(int busNumber, int programIndex)
{
	mpc.getDrum(busNumber - 1).setProgram(programIndex);
}

int Sampler::getDrumBusProgramIndex(int busNumber)
{
	return mpc.getDrum(busNumber - 1).getProgram();
}

std::shared_ptr<Sound> Sampler::getClickSound()
{
	return clickSound;
}

int Sampler::checkExists(std::string soundName)
{
	for (int i = 0; i < getSoundCount(); i++)
	{
		if (StrUtil::eqIgnoreCase(StrUtil::replaceAll(soundName, ' ', ""), getSoundName(i)))
			return i;
	}
	return -1;
}

void Sampler::selectPreviousSound()
{
    nudgeSoundIndex(false);
}

void Sampler::selectNextSound()
{
    nudgeSoundIndex(true);
}

std::weak_ptr<Sound> Sampler::copySound(std::weak_ptr<Sound> source)
{
	auto sound = source.lock();
	auto newSound = addSound(sound->getSampleRate());
	newSound->setName(sound->getName());
	newSound->setLoopEnabled(sound->isLoopEnabled());
	auto dest = newSound->getSampleData();
	auto src = sound->getSampleData();
	dest->reserve(src->size());
	copy(src->begin(), src->end(), back_inserter(*dest));
	newSound->setMono(sound->isMono());
	newSound->setEnd(sound->getEnd());
	newSound->setStart(sound->getStart());
	newSound->setLoopTo(sound->getLoopTo());
	return newSound;
}

void Sampler::copyProgram(const int sourceIndex, const int destIndex)
{
	if (programs[destIndex])
		programs[destIndex].reset();

	auto src = programs[sourceIndex];
	auto dest = addProgram(destIndex).lock();

	dest->setMidiProgramChange(dest->getMidiProgramChange());
	dest->setName(src->getName());
	
	for (int i = 0; i < 64; i++)
	{
		auto copy = dynamic_cast<NoteParameters*>(src->getNoteParameters(i + 35))->clone(i);
		dest->setNoteParameters(i, copy);
		
		auto mc1 = dest->getIndivFxMixerChannel(i);
		auto mc2 = src->getIndivFxMixerChannel(i);
		mc1->setFollowStereo(mc2->isFollowingStereo());
		mc1->setFxPath(mc2->getFxPath());
		mc1->setFxSendLevel(mc2->getFxSendLevel());
		mc1->setOutput(mc2->getOutput());
		mc1->setVolumeIndividualOut(mc2->getVolumeIndividualOut());

		auto mc3 = dest->getStereoMixerChannel(i);
		auto mc4 = src->getStereoMixerChannel(i);
		mc3->setLevel(mc4->getLevel());
		mc3->setPanning(mc4->getPanning());

		auto srcPad = src->getPad(i);
		auto destPad = dest->getPad(i);
		destPad->setNote(srcPad->getNote());
	}

	auto srcSlider = src->getSlider();
	auto destSlider = dest->getSlider();
	destSlider->setAssignNote(srcSlider->getNote());
	destSlider->setAttackHighRange(srcSlider->getAttackHighRange());
	destSlider->setAttackLowRange(srcSlider->getAttackLowRange());
	destSlider->setControlChange(srcSlider->getControlChange());
	destSlider->setDecayHighRange(srcSlider->getDecayHighRange());
	destSlider->setDecayLowRange(srcSlider->getDecayLowRange());
	destSlider->setFilterHighRange(srcSlider->getFilterHighRange());
	destSlider->setFilterLowRange(srcSlider->getFilterLowRange());
	destSlider->setParameter(srcSlider->getParameter());
	destSlider->setTuneHighRange(srcSlider->getTuneHighRange());
	destSlider->setTuneLowRange(srcSlider->getTuneLowRange());
}

int Sampler::getUsedProgram(int startIndex, bool up) {
	auto res = startIndex;

	if (up)
	{
		for (int i = startIndex + 1; i < programs.size(); i++)
		{
			if (programs[i])
			{
				res = i;
				break;
			}
		}
	}
	else
	{
		for (int i = startIndex - 1; i >= 0; i--)
		{
			if (programs[i])
			{
				res = i;
				break;
			}
		}
	}

	return res;
}

void Sampler::setPlayX(int i)
{
	if (i < 0 || i > 4)
	{
		return;
	}

    playXMode = i;
}

int Sampler::getPlayX()
{
	return playXMode;
}

bool Sampler::isSoundNameOccupied(const std::string& name)
{
	for (auto& s : sounds)
		if (StrUtil::eqIgnoreCase(StrUtil::trim(s->getName()), StrUtil::trim(name)))
			return true;

	return false;
}

std::vector<std::pair<std::shared_ptr<Sound>, int>> Sampler::getSoundsSortedByName()
{
    std::vector<std::pair<std::shared_ptr<Sound>, int>> result;

    for (int i = 0; i < sounds.size(); i++)
    {
        result.push_back({sounds[i], i});
    }

    std::sort(result.begin(),
              result.end(),
              [](const std::pair<std::shared_ptr<Sound>, int> s1, const std::pair<std::shared_ptr<Sound>, int> s2) -> bool {
                  return s1.first->getName() < s2.first->getName();
              });

    return result;
}

std::vector<std::pair<std::shared_ptr<Sound>,int>> Sampler::getSoundsSortedBySize()
{
    std::vector<std::pair<std::shared_ptr<Sound>, int>> result;

    for (int i = 0; i < sounds.size(); i++)
    {
        result.push_back({sounds[i], i});
    }

    std::sort(result.begin(),
              result.end(),
              [](const std::pair<std::shared_ptr<Sound>, int> s1, const std::pair<std::shared_ptr<Sound>, int> s2) -> bool {
                  return s1.first->getFrameCount() < s2.first->getFrameCount();
              });

    return result;
}

std::vector<std::pair<std::shared_ptr<Sound>, int>> Sampler::getSortedSounds()
{
    if (soundSortingType == 0)
    {
        std::vector<std::pair<std::shared_ptr<Sound>, int>> result;

        for (int i = 0; i < sounds.size(); i++)
        {
            result.push_back({sounds[i], i});
        }

        return result;
    }
    else if (soundSortingType == 1)
    {
        return getSoundsSortedByName();
    }

    return getSoundsSortedBySize();
}