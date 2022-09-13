#include "Sampler.hpp"

#include <Mpc.hpp>
#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/WavInputStringStream.hpp>

#include <sampler/NoteParameters.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sound.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequence.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/ZoneScreen.hpp>
#include <lcdgui/screens/dialog/MetronomeSoundScreen.hpp>

#include <synth/SynthChannel.hpp>
#include <audio/server/NonRealTimeAudioServer.hpp>

#include <mpc/MpcBasicSoundPlayerChannel.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>
#include <mpc/MpcStereoMixerChannel.hpp>
#include <mpc/MpcIndivFxMixerChannel.hpp>

#include <lang/StrUtil.hpp>

#include <thirdp/libsamplerate/samplerate.h>

#include <functional>

#include <cmrc/cmrc.hpp>
#include <string_view>

CMRC_DECLARE(mpc);

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::sampler;
using namespace ctoot::mpc;
using namespace moduru::lang;

Sampler::Sampler(mpc::Mpc& mpc)
	: mpc(mpc)
{
}

std::weak_ptr<Sound> Sampler::getPreviewSound()
{
    return sounds[sounds.size() - 1];
}

std::weak_ptr<Sound> Sampler::getSound(int index)
{
    if (index < 0 || index >= sounds.size())
        return {};

    return sounds[index];
}

std::weak_ptr<Program> Sampler::getProgram(int index)
{
    return programs[index];
}

void Sampler::setSoundIndex(int i)
{
	if (i < 0 || i >= sounds.size())
		return;

	soundIndex = i;

	auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");
	zoneScreen->initZones();
}

int Sampler::getSoundIndex()
{
	return soundIndex;
}

std::weak_ptr<Sound> Sampler::getSound()
{
	if (soundIndex < 0)
	{
		if (sounds.size() == 0)
			return {};

		// This is a bit of a hack, depending on what the real 2KXL does.
		// For now this will work fine to get some kind of sane index here after loading an APS, PGM, SND or WAV into an empty sampler.
		soundIndex = 0;
	}

	if (soundIndex >= sounds.size())
		return {};

	return sounds[soundIndex];
}

std::string Sampler::getPreviousScreenName()
{
	return previousScreenName;
}

void Sampler::setPreviousScreenName(std::string s)
{
	previousScreenName = s;
}

void Sampler::setInputLevel(int i)
{
	if (i < 0 || i > 100)
		return;

	inputLevel = i;
}

int Sampler::getInputLevel()
{
	return inputLevel;
}

std::vector<std::weak_ptr<MpcStereoMixerChannel>> Sampler::getDrumStereoMixerChannels(int i)
{
	return mpc.getDrums()[i]->getStereoMixerChannels();
}

std::vector<std::weak_ptr<MpcIndivFxMixerChannel>> Sampler::getDrumIndivFxMixerChannels(int i)
{
	return mpc.getDrums()[i]->getIndivFxMixerChannels();
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

std::vector<int>* Sampler::getAutoChromaticAssign()
{
	return &autoChromaticAssign;
}

void Sampler::init()
{
    initMasterPadAssign = Pad::getPadNotes(mpc);

	auto program = addProgram().lock();
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
    clickSound = std::make_shared<Sound>();
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
	autoChromaticAssign = std::vector<int>(64);

	for (int i = 0; i < 64; i++)
		autoChromaticAssign[i] = i;
}

void Sampler::playMetronome(mpc::sequencer::NoteEvent* event, int framePos)
{
	auto metronomeSoundScreen = mpc.screens->get<MetronomeSoundScreen>("metronome-sound");
	auto soundNumber = -2;

	auto velocity = event->getVelocity();

	if (metronomeSoundScreen->getSound() != 0)
	{
		auto program = mpc.getDrum(metronomeSoundScreen->getSound() - 1)->getProgram();
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
	
	mpc.getBasicPlayer()->mpcNoteOn(soundNumber, velocity, framePos);
}

void Sampler::playPreviewSample(int start, int end, int loopTo, int overlapMode)
{
	if (sounds.size() == 0)
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
	mpc.getBasicPlayer()->noteOn(-3, 127);
	previewSound->setStart(oldStart);
	previewSound->setEnd(oldEnd);
	previewSound->setLoopTo(oldLoopTo);
}

std::weak_ptr<MpcProgram> Sampler::getMpcProgram(int index)
{
	return programs[index];
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
	if (programs[i])
		return std::weak_ptr<Program>();

	programs[i] = std::make_shared<Program>(mpc, this);
	return programs[i];
}

std::weak_ptr<Program> Sampler::addProgram()
{
	for (auto& p : programs)
	{
		if (!p)
		{
			p = std::make_shared<Program>(mpc, this);
			return p;
		}
	}
	return std::weak_ptr<Program>();
}

void Sampler::deleteProgram(std::weak_ptr<Program> _program)
{
	auto program = _program.lock();
	
	int firstValidProgram;
	
	for (firstValidProgram = 0; firstValidProgram < programs.size(); firstValidProgram++)
	{
		if (programs[firstValidProgram] && programs[firstValidProgram] != program)
		{
			for (auto& bus : mpc.getDrums())
			{
				if (programs[bus->getProgram()] == program)
					bus->setProgram(firstValidProgram);
			}
	
			break;
		}
	}

	for (auto&& p : programs)
	{
		if (p == program)
		{
			p.reset();
			break;
		}
	}
}

std::vector<std::weak_ptr<Sound>> Sampler::getSounds()
{
	auto res = std::vector<std::weak_ptr<Sound>>();

	for (auto& s : sounds)
		res.push_back(s);

	return res;
}

std::weak_ptr<Sound> Sampler::addSound()
{
	return addSound(44100);
}

std::weak_ptr<Sound> Sampler::addSound(int sampleRate)
{
	auto res = std::make_shared<Sound>(sampleRate, sounds.size());
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

std::vector<std::string> Sampler::getSoundNames()
{
    std::vector<std::string> result;
    for (int i = 0; i < getSoundCount(); i++)
        result.push_back(sounds[i]->getName());
    return result;
}


std::string Sampler::getPadName(int i)
{
	return padNames[i];
}

std::vector<std::weak_ptr<Program>> Sampler::getPrograms()
{
	auto res = std::vector<std::weak_ptr<Program>>();

	for (auto& p : programs)
		res.push_back(p);

	return res;
}

void Sampler::replaceProgram(std::weak_ptr<Program> p, int index)
{
	auto sourceProgram = p.lock();
	auto destProgram = programs[index];
	destProgram.swap(sourceProgram);
	deleteProgram(sourceProgram);
}

void Sampler::deleteAllPrograms(bool init)
{
	for (auto& p : programs)
		p.reset();

	if (!init)
		return;

	addProgram().lock()->setName("NewPgm-A");
	checkProgramReferences();
}

void Sampler::checkProgramReferences()
{
	auto lSequencer = mpc.getSequencer().lock();
	auto t = lSequencer->getActiveSequence().lock()->getTrack(lSequencer->getActiveTrackIndex()).lock();
	auto bus = t->getBus();

	for (int i = 0; i < 4; i++)
	{
		auto pgm = getDrumBusProgramNumber(bus);
		
		if (!programs[pgm])
		{
			// check what real MPC does in this condition
			for (int i = 0; i < 24; i++)
			{
				if (programs[i])
					setDrumBusProgramNumber(bus, i);
			}
		}
	}
}

std::vector<float>* Sampler::getClickSample()
{
	return &clickSample;
}

std::weak_ptr<MpcSound> Sampler::getMpcSound(int index)
{
    if (index < 0 || index >= sounds.size())
        return {};

	return sounds[index];
}

std::weak_ptr<MpcSound> Sampler::getMpcPreviewSound()
{
	if (sounds.size() == 0)
		return std::weak_ptr<Sound>();

	return sounds[sounds.size() - 1];
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

void Sampler::sort()
{
	if (sounds.empty())
		return;

	soundSortingType++; // This should be optional. Only when called by F1/F2/F3/F4 it should do this.
	
	if (soundSortingType > 2)
		soundSortingType = 0;

	auto currentSoundMemoryIndex = soundIndex == -1 ? -1 : sounds[soundIndex]->getMemoryIndex();

	std::vector<std::string> oldSoundNames;
	std::transform(begin(sounds), end(sounds), std::back_inserter(oldSoundNames), [](std::shared_ptr<Sound> sound) { return sound->getName(); });

	switch (soundSortingType)
	{
	case 0:
		stable_sort(sounds.begin(), sounds.end(), compareMemoryIndex);
		break;
	case 1:
		stable_sort(sounds.begin(), sounds.end(), compareName);
		break;
	case 2:
		stable_sort(sounds.begin(), sounds.end(), compareSize);
		break;
	}

	std::vector<std::string> newSoundNames;
	std::transform(begin(sounds), end(sounds), std::back_inserter(newSoundNames), [](std::shared_ptr<Sound> sound) { return sound->getName(); });

	std::vector<NoteParameters*> correctedNoteParameters;

	for (auto& program : programs)
	{
		if (!program)
			continue;

		for (auto noteParameters : program->getNotesParameters())
		{
			if (find(correctedNoteParameters.begin(), correctedNoteParameters.end(), noteParameters) != correctedNoteParameters.end())
				continue;

			if (noteParameters->getSoundIndex() != -1)
			{
				auto name = *find(begin(oldSoundNames), end(oldSoundNames), oldSoundNames[noteParameters->getSoundIndex()]);
				auto newNameItr = find(begin(newSoundNames), end(newSoundNames), name);
				auto newIndex = std::distance(begin(newSoundNames), newNameItr);
				noteParameters->setSoundIndex(newIndex);
				correctedNoteParameters.push_back(noteParameters);
			}
		}
	}

	for (int i = 0; i < sounds.size(); i++)
	{
		if (sounds[i]->getMemoryIndex() == currentSoundMemoryIndex) {
			setSoundIndex(i);
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

void Sampler::stopAllVoices()
{
	if (!mpc.getAudioMidiServices().lock()->getAudioServer()->isRunning())
		return;
	
	mpc.getBasicPlayer()->allSoundOff();
	
	for (auto m : mpc.getDrums())
		m->allSoundOff();
}

void Sampler::stopAllVoices(int frameOffset)
{
	dynamic_cast<MpcSoundPlayerChannel*>(mpc.getDrums()[0])->allSoundOff(frameOffset);
}

void Sampler::finishBasicVoice()
{
	mpc.getBasicPlayer()->finishVoice();
}

void Sampler::playX()
{
	auto sound = sounds[soundIndex];
	auto start = 0;
	auto end = sound->getSampleData()->size() - 1;

	if (!sound->isMono())
		end *= 0.5;

	auto fullEnd = end;

	if (playX_ == 1)
	{
		auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");
		auto zone = zoneScreen->getZone();
		start = zone[0];
		end = zone[1];
	}
	else if (playX_ == 2)
	{
		end = sound->getStart();
	}
	else if (playX_ == 3)
	{
		end = sound->getLoopTo();
	}
	else if (playX_ == 4)
	{
		start = sound->getEnd();
		end = fullEnd;
	}

	int oldStart = sound->getStart();
	int oldEnd = sound->getEnd();
	sound->setStart(start);
	sound->setEnd(end);
	mpc.getBasicPlayer()->noteOn(-4, 127);
	sound->setStart(oldStart);
	sound->setEnd(oldEnd);
}

std::weak_ptr<MpcSound> Sampler::getPlayXSound()
{
	return sounds[soundIndex];
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
	auto res = addOrIncreaseNumber2(s);
	bool exists = true;
	
	while (exists)
	{
		exists = false;

		for (int i = 0; i < getSoundCount(); i++)
		{
			if (getSoundName(i).compare(res) == 0)
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
	auto lastValidPad = mpc.getPad();
	
	if (lastValidPad == -1)
		lastValidPad = mpc.getPrevPad();

	return program->getPad(lastValidPad);
}

NoteParameters* Sampler::getLastNp(Program* program)
{
	auto lastValidNote = mpc.getNote();
	
	if (lastValidNote == 34)
		lastValidNote = mpc.getPrevNote();

	return dynamic_cast<mpc::sampler::NoteParameters*>(program->getNoteParameters(lastValidNote));
}

std::vector<std::weak_ptr<Sound>> Sampler::getUsedSounds()
{
	std::set<std::weak_ptr<Sound>, std::owner_less<std::weak_ptr<Sound>>> usedSounds;

	for (auto& p : programs)
	{
		if (!p)
			continue;
	
		for (auto& nn : p->getNotesParameters())
		{
			if (nn->getSoundIndex() != -1)
				usedSounds.emplace(sounds[nn->getSoundIndex()]);
		}
	}
	return std::vector<std::weak_ptr<Sound>>(begin(usedSounds), end(usedSounds));
}

int Sampler::getUnusedSampleCount()
{
	return (int)(sounds.size() - getUsedSounds().size());
}

void Sampler::purge()
{
	auto usedSounds = getUsedSounds();

	for (int i = 0; i < sounds.size(); i++)
	{
		auto maybeUsedSound = sounds[i];
		const auto pos = find_if(begin(usedSounds), end(usedSounds), [&maybeUsedSound](const std::weak_ptr<Sound>& sound) {
			return sound.lock() == maybeUsedSound;
		});
		
		if (pos == usedSounds.end())
		{
			deleteSound(maybeUsedSound);
			i--;
		}
	}
}


void Sampler::deleteSound(int soundIndex)
{
	deleteSound(sounds[soundIndex]);
}

void Sampler::deleteSound(std::weak_ptr<Sound> sound)
{
	auto index = sound.lock()->getMemoryIndex();

    for (auto& p : programs)
    {
        if (!p) continue;

        for (auto& n : p->getNotesParameters())
        {
            if (n->getSoundIndex() == index)
            {
                n->setSoundIndex(-1);
            }
        }
    }

	for (int i = 0; i < sounds.size(); i++)
	{
		if (sounds[i] == sound.lock())
		{
			sounds.erase(sounds.begin() + i);
			break;
		}
	}

	stable_sort(sounds.begin(), sounds.end(), compareMemoryIndex);

	for (int i = 0; i < sounds.size(); i++)
	{
		auto oldMemoryIndex = sounds[i]->getMemoryIndex();

		sounds[i]->setMemoryIndex(i);

		for (auto& program : programs)
		{
			if (!program) continue;

			for (auto noteParameters : program->getNotesParameters())
			{
				if (noteParameters->getSoundIndex() == oldMemoryIndex)
				{
					noteParameters->setSoundIndex(i);
				}
			}
		}
	}

	switch (soundSortingType)
	{
	case 0:
		stable_sort(sounds.begin(), sounds.end(), compareMemoryIndex);
		break;
	case 1:
		stable_sort(sounds.begin(), sounds.end(), compareName);
		break;
	case 2:
		stable_sort(sounds.begin(), sounds.end(), compareSize);
		break;
	}

	if (soundIndex >= sounds.size())
		soundIndex--;
	
	if (soundIndex < 0)
		soundIndex = 0;
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

void Sampler::setDrumBusProgramNumber(int busNumber, int programNumber)
{
	mpc.getDrums()[busNumber - 1]->setProgram(programNumber);
}

int Sampler::getDrumBusProgramNumber(int busNumber)
{
	return mpc.getDrums()[busNumber - 1]->getProgram();
}

MpcSoundPlayerChannel* Sampler::getDrum(int i)
{
	return mpc.getDrum(i);
}

std::weak_ptr<MpcSound> Sampler::getClickSound()
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

int Sampler::getNextSoundIndex(int j, bool up)
{
	auto inc = up ? 1 : -1;
	if (j + inc < -1 || j + inc > getSoundCount() - 1)
	{
		return j;
	}
	return j + inc;
}

void Sampler::selectPreviousSound()
{
	setSoundIndex(getNextSoundIndex(soundIndex, false));
}

void Sampler::selectNextSound()
{
	setSoundIndex(getNextSoundIndex(soundIndex, true));
}

std::weak_ptr<Sound> Sampler::copySound(std::weak_ptr<Sound> source)
{
	auto sound = source.lock();
	auto newSound = addSound(sound->getSampleRate()).lock();
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
		
		auto mc1 = dest->getIndivFxMixerChannel(i).lock();
		auto mc2 = src->getIndivFxMixerChannel(i).lock();
		mc1->setFollowStereo(mc2->isFollowingStereo());
		mc1->setFxPath(mc2->getFxPath());
		mc1->setFxSendLevel(mc2->getFxSendLevel());
		mc1->setOutput(mc2->getOutput());
		mc1->setVolumeIndividualOut(mc2->getVolumeIndividualOut());

		auto mc3 = dest->getStereoMixerChannel(i).lock();
		auto mc4 = src->getStereoMixerChannel(i).lock();
		mc3->setLevel(mc4->getLevel());
		mc3->setPanning(mc4->getPanning());
		mc3->setStereo(mc4->isStereo());

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

bool Sampler::compareMemoryIndex(std::weak_ptr<Sound> a, std::weak_ptr<Sound> b) {
	return a.lock()->getMemoryIndex() < b.lock()->getMemoryIndex();
}

bool Sampler::compareName(std::weak_ptr<Sound> a, std::weak_ptr<Sound> b) {
	return a.lock()->getName() < b.lock()->getName();
}

bool Sampler::compareSize(std::weak_ptr<Sound> a, std::weak_ptr<Sound> b) {
	return a.lock()->getFrameCount() < b.lock()->getFrameCount();
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

	playX_ = i;
}

int Sampler::getPlayX()
{
	return playX_;
}

bool Sampler::isSoundNameOccupied(const std::string& name)
{
	for (auto& s : sounds)
		if (StrUtil::eqIgnoreCase(StrUtil::trim(s->getName()), StrUtil::trim(name)))
			return true;

	return false;
}
