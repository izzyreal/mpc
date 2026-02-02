#include "Sampler.hpp"

#include "Mpc.hpp"
#include "MpcSpecs.hpp"
#include "engine/EngineHost.hpp"
#include "audiomidi/WavInputStringStream.hpp"

#include "controller/ClientEventController.hpp"
#include "sampler/NoteParameters.hpp"
#include "sampler/Pad.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sound.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequencer.hpp"

#include "lcdgui/screens/ZoneScreen.hpp"
#include "lcdgui/screens/dialog/MetronomeSoundScreen.hpp"

#include "engine/audio/server/NonRealTimeAudioServer.hpp"

#include "engine/PreviewSoundPlayer.hpp"
#include "engine/StereoMixer.hpp"
#include "engine/IndivFxMixer.hpp"

#include "StrUtil.hpp"

#include "MpcResourceUtil.hpp"
#include "engine/Voice.hpp"

#include <memory>
#include <set>

#include <samplerate.h>

#include "Logger.hpp"
#include "performance/PerformanceManager.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::sampler;
using namespace mpc::sequencer;
using namespace mpc::engine;

Sampler::Sampler(
    Mpc &mpc, GetProgramFn &&getSnapshot,
    const std::function<void(performance::PerformanceMessage &&)> &dispatch)
    : mpc(mpc), getSnapshot(std::move(getSnapshot)), dispatch(dispatch)
{
}

std::shared_ptr<Sound> Sampler::getPreviewSound()
{
    return sounds.back();
}

std::shared_ptr<Sound> Sampler::getSound(const int index)
{
    if (index < 0 || index >= sounds.size())
    {
        return {};
    }

    return sounds[index];
}

void Sampler::replaceSound(const int index,
                           const std::shared_ptr<Sound> &newSound)
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

std::shared_ptr<Program> Sampler::getProgram(const int index)
{
    return programs[index];
}

void Sampler::nudgeSoundIndex(const bool up)
{
    if (up)
    {
        if (soundIndex >= sounds.size() - 1)
        {
            return;
        }

        soundIndex++;
        return;
    }

    if (soundIndex == 0)
    {
        return;
    }

    soundIndex--;
}

void Sampler::setSoundIndex(const int i)
{
    soundIndex =
        std::clamp(i, 0, std::max(0, static_cast<int>(sounds.size() - 1)));
}

short Sampler::getSoundIndex() const
{
    return soundIndex;
}

std::shared_ptr<Sound> Sampler::getSound()
{
    if (soundIndex < 0)
    {
        if (sounds.empty())
        {
            return {};
        }

        // This is a bit of a hack, depending on what the real 2KXL does.
        // For now this will work fine to get some kind of sane index here after
        // loading an APS, PGM, SND or WAV into an empty sampler.
        soundIndex = 0;
    }

    if (soundIndex >= sounds.size())
    {
        return {};
    }

    return getSortedSounds()[soundIndex].first;
}

std::string Sampler::getPreviousScreenName()
{
    return previousScreenName;
}

void Sampler::setPreviousScreenName(const std::string &s)
{
    previousScreenName = s;
}

std::vector<mpc::DrumNoteNumber> *Sampler::getInitMasterPadAssign()
{
    return &initMasterPadAssign;
}

std::vector<mpc::DrumNoteNumber> *Sampler::getMasterPadAssign()
{
    return &masterPadAssign;
}

void Sampler::setMasterPadAssign(const std::vector<DrumNoteNumber> &v)
{
    masterPadAssign = v;
}

void Sampler::init()
{
    initMasterPadAssign = Pad::getPadNotes(mpc);

    for (int i = 0; i < Mpc2000XlSpecs::MAX_PROGRAM_COUNT; ++i)
    {
        programs[i] = std::make_shared<Program>(ProgramIndex(i), mpc, this,
                                                getSnapshot, dispatch);
    }

    const auto program = createNewProgramAddFirstAvailableSlot().lock();
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

    clickSound = std::make_shared<Sound>(44100);
    clickSound->setMono(true);
    clickSound->setLevel(100);

    auto clickData = MpcResourceUtil::get_resource_data("audio/click.wav");

    if (clickData.size() != 146)
    {
        MLOG(
            "Warning: click.wav does not exist or doesn't have the correct "
            "size!");
    }
    else
    {
        auto stream = wav_init_istringstream(&clickData[0], clickData.size());
        int sampleRate, validBits, numChannels, numFrames;
        wav_read_header(stream, sampleRate, validBits, numChannels, numFrames);

        if (numChannels == 1 && validBits == 16)
        {
            for (int i = 0; i < numFrames; i++)
            {
                const float frame = wav_get_LE(stream, 2) / 32768.0;
                clickSound->insertFrame(std::vector{frame},
                                        clickSound->getFrameCount());
            }
        }
        clickSound->setEnd(numFrames);
    }

    masterPadAssign = initMasterPadAssign;
}

void Sampler::playMetronome(unsigned int velocity, const int framePos) const
{
    const auto metronomeSoundScreen =
        mpc.screens->get<ScreenId::MetronomeSoundScreen>();

    if (metronomeSoundScreen->getSound() == 0)
    {
        velocity *= metronomeSoundScreen->getVolume() * 0.01;
        mpc.getEngineHost()->getPreviewSoundPlayer()->playSound(
            CLICK_SOUND, velocity, framePos);
        return;
    }

    const auto drumBus = mpc.getSequencer()->getBus<DrumBus>(
        busIndexToBusType(metronomeSoundScreen->getSound()));

    assert(drumBus);

    const auto programIndex = drumBus->getProgramIndex();

    const auto accent = velocity == 127;
    velocity = accent ? metronomeSoundScreen->getAccentVelo()
                      : metronomeSoundScreen->getNormalVelo();
    const auto pad = accent ? metronomeSoundScreen->getAccentPad()
                            : metronomeSoundScreen->getNormalPad();
    const auto note =
        programs[programIndex]->getNoteFromPad(ProgramPadIndex(pad));
    const auto soundNumber =
        programs[programIndex]->getNoteParameters(note)->getSoundIndex();
    mpc.getEngineHost()->getPreviewSoundPlayer()->playSound(soundNumber,
                                                            velocity, framePos);
}

void Sampler::playPreviewSample(const int start, const int end,
                                const int loopTo) const
{
    if (sounds.empty())
    {
        return;
    }

    const auto previewSound = sounds[sounds.size() - 1];
    const auto oldStart = previewSound->getStart();
    const auto oldEnd = previewSound->getEnd();
    const auto oldLoopTo = previewSound->getLoopTo();
    previewSound->setStart(start);
    previewSound->setEnd(end);
    previewSound->setLoopTo(loopTo);
    mpc.getEngineHost()->getPreviewSoundPlayer()->playSound(PREVIEW_SOUND, 127,
                                                            0);
    previewSound->setStart(oldStart);
    previewSound->setEnd(oldEnd);
    previewSound->setLoopTo(oldLoopTo);
}

int Sampler::getProgramCount() const
{
    int res = 0;

    for (auto &p : programs)
    {
        if (p->isUsed())
        {
            res++;
        }
    }

    return res;
}

std::weak_ptr<Program> Sampler::addProgram(const int i)
{
    assert(!programs[i]->isUsed());
    programs[i]->setUsed();
    return programs[i];
}

std::weak_ptr<Program> Sampler::createNewProgramAddFirstAvailableSlot()
{
    const bool repairDrumPrograms = getProgramCount() == 0;

    for (int programIndex = 0; programIndex < Mpc2000XlSpecs::MAX_PROGRAM_COUNT;
         ++programIndex)
    {
        if (auto &p = programs[programIndex]; !p->isUsed())
        {
            p->setUsed();

            if (repairDrumPrograms)
            {
                for (int i = 0; i < Mpc2000XlSpecs::DRUM_BUS_COUNT; i++)
                {
                    const auto drumBus =
                        mpc.getSequencer()->getDrumBus(DrumBusIndex(i));
                    drumBus->setProgramIndex(ProgramIndex(0));
                }
            }

            return p;
        }
    }

    return std::weak_ptr<Program>();
}

void Sampler::deleteProgram(const std::weak_ptr<Program> &program) const
{
    for (auto &&p : programs)
    {
        if (p == program.lock())
        {
            p->resetToDefaultValues();
            break;
        }
    }

    mpc.getPerformanceManager().lock()->enqueue(performance::PerformanceMessage(
        performance::RepairProgramReferences{}));
}

std::vector<std::shared_ptr<Sound>> &Sampler::getSounds()
{
    return sounds;
}

std::shared_ptr<Sound> Sampler::addSound()
{
    return addSound(44100);
}

/*
 * Adds a sound to the sampler's sound directory, if there is room for it.
 * On the MPC2000XL, the sound directory can hold a maximum of 256 sounds at a
 * time.
 * If there is room, a shared_ptr to the added sound, with no sample data, is
 * returned. The sound has been initialized to the sample rate provided by the
 * caller. If there is no room, an empty shared_ptr is ultimately returned.
 * But in this case, first the PopupScreen is opened with a message to the user
 * that the sound directory is full. The popup screen awaits interaction (a
 * button push), to finally return to the previous screen (before the popup was
 * opened).
 */
std::shared_ptr<Sound> Sampler::addSound(const int sampleRate)
{
    if (sounds.size() >= Mpc2000XlSpecs::MAX_SOUND_COUNT_IN_MEMORY)
    {
        mpc.getLayeredScreen()->showPopupAndAwaitInteraction(
            "Sound directory full(256max)");
        return {};
    }

    sounds.emplace_back(std::make_shared<Sound>(sampleRate));
    return sounds.back();
}

int Sampler::getSoundCount() const
{
    return sounds.size();
}

std::string Sampler::getSoundName(const int i) const
{
    return sounds[i]->getName();
}

std::string Sampler::getPadName(const int i)
{
    if (i < 0)
    {
        return "OFF";
    }
    return padNames[i];
}

std::vector<std::weak_ptr<Program>> Sampler::getPrograms() const
{
    auto res = std::vector<std::weak_ptr<Program>>();

    for (auto &p : programs)
    {
        res.push_back(p);
    }

    return res;
}

void Sampler::deleteAllPrograms(const bool createDefaultProgram)
{
    for (auto &p : programs)
    {
        deleteProgram(p);
    }

    if (createDefaultProgram)
    {
        utils::SimpleAction callback([this]
        {
            createNewProgramAddFirstAvailableSlot().lock()->setName("NewPgm-A");
        });

        mpc.getPerformanceManager().lock()->enqueueCallback(std::move(callback));
    }
}

void Sampler::trimSample(const int sampleNumber, const int start,
                         const int end) const
{
    const auto s = sounds[sampleNumber];
    Sampler::trimSample(s, start, end);
}

void Sampler::trimSample(const std::weak_ptr<Sound> &sound, const int start,
                         int end)
{
    const auto s = sound.lock();
    std::vector<float> newData = *s->getSampleData();
    const auto frameCount = s->getFrameCount();

    if (end > frameCount)
    {
        end = frameCount;
    }

    if (s->isMono())
    {
        newData.erase(newData.begin() + end, newData.end());
        newData.erase(newData.begin(), newData.begin() + start);
    }
    else
    {
        const int startRight = start + frameCount;
        const int endRight = end + frameCount;

        newData.erase(newData.begin() + endRight, newData.end());
        newData.erase(newData.begin() + frameCount,
                      newData.begin() + startRight);
        newData.erase(newData.begin() + end, newData.begin() + frameCount);
        newData.erase(newData.begin(), newData.begin() + start);
    }

    s->setSampleData(std::make_shared<std::vector<float>>(newData));

    s->setStart(0);
    s->setEnd(s->getFrameCount());
    s->setLoopTo(s->getFrameCount());
}

void Sampler::deleteSection(const unsigned int sampleNumber,
                            const unsigned int start,
                            const unsigned int end) const
{
    const auto s = sounds[sampleNumber];
    std::vector<float> newData = *s->getSampleData();
    const auto frameCount = s->getFrameCount();

    if (!s->isMono())
    {
        const unsigned int startRight = start + frameCount;
        const unsigned int endRight = end + frameCount;
        newData.erase(newData.begin() + startRight, newData.begin() + endRight);
    }

    newData.erase(newData.begin() + start, newData.begin() + end);
    s->setSampleData(std::make_shared<std::vector<float>>(newData));
}

std::string Sampler::getSoundSortingTypeName() const
{
    if (soundSortingType == 0)
    {
        return "MEMORY";
    }
    if (soundSortingType == 1)
    {
        return "NAME";
    }
    return "SIZE";
}

void Sampler::switchToNextSoundSortType()
{
    const auto s = getSound();

    if (soundSortingType++ >= 2)
    {
        soundSortingType = 0;
    }

    const auto sortedSounds = getSortedSounds();

    for (int i = 0; i < sortedSounds.size(); i++)
    {
        if (sortedSounds[i].first == s)
        {
            soundIndex = i;
            break;
        }
    }
}

void Sampler::deleteAllSamples()
{
    sounds.clear();

    for (const auto &p : programs)
    {
        if (!p->isUsed())
        {
            continue;
        }

        for (const auto &n : p->getNotesParameters())
        {
            n->setSoundIndex(-1);
        }
    }

    soundIndex = 0;
}

void Sampler::process12Bit(std::vector<float> &data)
{
    for (auto j = 0; j < data.size(); j++)
    {
        if (data[j] != 0.0f)
        {
            auto fShort = static_cast<int16_t>(data[j] * 32767.4999999);

            if (data[j] > 0.9999999f)
            {
                fShort = 32767;
            }

            int16_t newShort = fShort;
            newShort &= ~(1 << 0);
            newShort &= ~(1 << 1);
            newShort &= ~(1 << 2);
            newShort &= ~(1 << 3);

            data[j] = static_cast<float>(newShort / 32767.4999999);
        }
        else
        {
            data[j] = 0;
        }
    }
}

void Sampler::process8Bit(std::vector<float> &data)
{
    for (auto j = 0; j < data.size(); j++)
    {
        if (data[j] != 0.0f)
        {
            float f = data[j];

            if (f < -1)
            {
                f = -1;
            }
            else if (f > 1)
            {
                f = 1;
            }

            const unsigned short ushort =
                static_cast<signed short>((f + 1) * 32767.4999999);
            const unsigned char eightBit = ushort >> 8;
            const signed sshort = (eightBit - 128) << 8;

            f = static_cast<float>(sshort / 32767.49999999);

            if (f < -1)
            {
                f = -1;
            }
            else if (f > 1)
            {
                f = 1;
            }

            data[j] = f;
        }
    }
}

void Sampler::resample(const std::shared_ptr<const std::vector<float>> &data,
                       const int sourceRate,
                       const std::shared_ptr<Sound> &destSnd)
{
    const auto inputFrameCount =
        static_cast<int>(destSnd->isMono() ? data->size() : data->size() / 2);
    const auto srcRatio = static_cast<double>(destSnd->getSampleRate()) /
                          static_cast<double>(sourceRate);
    const auto outputFrameCount =
        static_cast<int>((floor)(inputFrameCount * srcRatio));
    const auto destinationSampleCount =
        destSnd->isMono() ? outputFrameCount : outputFrameCount * 2;

    SRC_DATA srcData;
    srcData.input_frames = inputFrameCount;
    srcData.src_ratio = srcRatio;

    srcData.output_frames = outputFrameCount;

    const auto destinationSampleData = destSnd->getMutableSampleData();
    destinationSampleData->resize(destinationSampleCount);

    const int numChannels = destSnd->isMono() ? 1 : 2;

    for (int i = 0; i < numChannels; i++)
    {
        const auto srcArray = &(*data)[i * inputFrameCount];
        srcData.data_in = srcArray;

        float *destArray = &(*destinationSampleData)[i * outputFrameCount];
        srcData.data_out = destArray;

        if (const auto error = src_simple(&srcData, 0, 1); error != 0)
        {
            const char *errormsg = src_strerror(error);
            std::string errorStr(errormsg);
            MLOG("libsamplerate error: " + errorStr);
        }
    }
}

void Sampler::stopAllVoices(const int frameOffset) const
{
    for (const auto &voice : mpc.getEngineHost()->getVoices())
    {
        if (voice->isFinished())
        {
            continue;
        }

        voice->startDecay(frameOffset);
    }
}

void Sampler::playX()
{
    const auto sound = getSortedSounds()[soundIndex].first;
    auto start = 0;
    auto end = sound->getSampleData()->size() - 1;

    if (!sound->isMono())
    {
        end *= 0.5;
    }

    const auto fullEnd = end;

    if (playXMode == 1)
    {
        const auto zoneScreen = mpc.screens->get<ScreenId::ZoneScreen>();
        const auto zone = zoneScreen->getZone();
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

    const int oldStart = sound->getStart();
    const int oldEnd = sound->getEnd();
    sound->setStart(start);
    sound->setEnd(end);
    mpc.getEngineHost()->getPreviewSoundPlayer()->playSound(PLAYX_SOUND, 127,
                                                            0);
    sound->setStart(oldStart);
    sound->setEnd(oldEnd);
}

std::shared_ptr<Sound> Sampler::getPlayXSound()
{
    return getSortedSounds()[soundIndex].first;
}

int Sampler::getFreeSampleSpace() const
{
    double freeSpace = 32620;

    for (auto &s : sounds)
    {
        freeSpace -= s->getSampleData()->size() * 2 / static_cast<double>(1024);
    }

    return static_cast<int>(floor(freeSpace));
}

int Sampler::getLastInt(const std::string &s)
{
    auto offset = s.length();

    for (int i = s.length() - 1; i >= 0; i--)
    {
        if (const auto c = s[i]; isdigit(c))
        {
            offset--;
        }
        else
        {
            if (offset == s.length())
            {
                return INT_MIN;
            }

            break;
        }
    }
    return stoi(s.substr(offset));
}

std::string Sampler::addOrIncreaseNumber(const std::string &s) const
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
                res = Sampler::addOrIncreaseNumber2(res);
                break;
            }
        }
    }

    return res;
}

std::string Sampler::addOrIncreaseNumber2(const std::string &s)
{
    int candidate = getLastInt(s);
    std::string res = s;

    if (candidate == INT_MIN)
    {
        if (res.length() >= 16)
        {
            res = res.substr(0, 15);
        }
        return res + std::to_string(1);
    }

    auto candidateStr = std::to_string(candidate);
    int candidateLength = candidateStr.length();

    res = res.substr(0, res.length() - candidateLength);

    candidate++;

    candidateStr = std::to_string(candidate);
    candidateLength = candidateStr.length();

    if (res.length() + candidateLength > 16)
    {
        res = res.substr(0, 16 - candidateLength);
    }

    res += candidateStr;
    return res;
}

std::vector<std::shared_ptr<Sound>> Sampler::getUsedSounds() const
{
    std::set<std::shared_ptr<Sound>> usedSounds;

    for (auto &p : programs)
    {
        if (!p->isUsed())
        {
            continue;
        }

        for (const auto &nn : p->getNotesParameters())
        {
            if (nn->getSoundIndex() != -1)
            {
                usedSounds.insert(sounds[nn->getSoundIndex()]);
            }
        }
    }
    return {begin(usedSounds), end(usedSounds)};
}

int Sampler::getUnusedSampleCount() const
{
    return static_cast<int>(sounds.size() - getUsedSounds().size());
}

void Sampler::purge()
{
    auto usedSounds = getUsedSounds();

    for (int i = sounds.size() - 1; i >= 0; i--)
    {
        auto maybeUnusedSound = sounds[i];

        const auto pos =
            find_if(begin(usedSounds), end(usedSounds),
                    [&maybeUnusedSound](const std::shared_ptr<Sound> &sound)
                    {
                        return sound == maybeUnusedSound;
                    });

        if (pos == usedSounds.end())
        {
            deleteSound(maybeUnusedSound);
        }
    }
}

void Sampler::deleteSound(const int deleteSoundIndex)
{
    deleteSound(sounds[deleteSoundIndex]);
}

void Sampler::deleteSoundWithoutRepairingPrograms(
    const std::shared_ptr<Sound> &sound)
{
    int index = -1;

    for (int i = 0; i < sounds.size(); i++)
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

void Sampler::deleteSound(const std::shared_ptr<Sound> &sound)
{
    int index = -1;

    for (int i = 0; i < sounds.size(); i++)
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

    mpc.getPerformanceManager().lock()->enqueue(performance::PerformanceMessage{
        performance::DeleteSoundAndReindex{index}});

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

std::vector<float> Sampler::mergeToStereo(const std::vector<float> &fa0,
                                          const std::vector<float> &fa1)
{
    const int newLengthFrames =
        fa0.size() > fa1.size() ? fa0.size() : fa1.size();
    std::vector<float> newSampleData(newLengthFrames * 2);

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
        else
        {
            newSampleData[i + newLengthFrames] = 0;
        }
    }

    return newSampleData;
}

void Sampler::mergeToStereo(
    const std::shared_ptr<const std::vector<float>> &sourceLeft,
    const std::shared_ptr<const std::vector<float>> &sourceRight,
    const std::shared_ptr<std::vector<float>> &dest)
{
    dest->clear();

    for (auto &f : *sourceLeft)
    {
        dest->push_back(f);
    }

    if (sourceLeft->size() < sourceRight->size())
    {
        const auto diff = sourceRight->size() - sourceLeft->size();

        for (int i = 0; i < diff; i++)
        {
            dest->push_back(0);
        }
    }

    for (auto &f : *sourceRight)
    {
        dest->push_back(f);
    }

    if (sourceRight->size() < sourceLeft->size())
    {
        const auto diff = sourceLeft->size() - sourceRight->size();
        for (int i = 0; i < diff; i++)
        {
            dest->push_back(0);
        }
    }
}

std::shared_ptr<Sound> Sampler::getClickSound()
{
    return clickSound;
}

int Sampler::checkExists(const std::string &soundName) const
{
    for (int i = 0; i < getSoundCount(); i++)
    {
        if (StrUtil::eqIgnoreCase(StrUtil::replaceAll(soundName, ' ', ""),
                                  getSoundName(i)))
        {
            return i;
        }
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

std::weak_ptr<Sound> Sampler::copySound(const std::weak_ptr<Sound> &source)
{
    const auto sound = source.lock();
    auto newSound = addSound(sound->getSampleRate());

    if (!newSound)
    {
        return {};
    }

    newSound->setName(sound->getName());
    newSound->setLoopEnabled(sound->isLoopEnabled());
    const auto dest = newSound->getMutableSampleData();
    const auto src = sound->getSampleData();
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
    if (programs[destIndex]->isUsed())
    {
        programs[destIndex]->resetToDefaultValues();
    }

    const auto src = programs[sourceIndex];
    const auto dest = addProgram(destIndex).lock();

    dest->setMidiProgramChange(dest->getMidiProgramChange());
    dest->setName(src->getName());

    for (int i = 0; i < Mpc2000XlSpecs::PROGRAM_PAD_COUNT; i++)
    {
        src->cloneNoteParameters(DrumNoteNumber(i + MinDrumNoteNumber),
                                 dest.get(),
                                 DrumNoteNumber(i + MinDrumNoteNumber));

        const auto mc1 = dest->getIndivFxMixerChannel(i);
        const auto mc2 = src->getIndivFxMixerChannel(i);
        mc1->setFollowStereo(mc2->isFollowingStereo());
        mc1->setFxPath(mc2->getFxPath());
        mc1->setFxSendLevel(mc2->getFxSendLevel());
        mc1->setOutput(mc2->getOutput());
        mc1->setVolumeIndividualOut(mc2->getVolumeIndividualOut());

        const auto mc3 = dest->getStereoMixerChannel(i);
        const auto mc4 = src->getStereoMixerChannel(i);
        mc3->setLevel(mc4->getLevel());
        mc3->setPanning(mc4->getPanning());

        const auto srcPad = src->getPad(i);
        const auto destPad = dest->getPad(i);
        destPad->setNote(srcPad->getNote());
    }

    const auto srcSlider = src->getSlider();
    const auto destSlider = dest->getSlider();
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

mpc::ProgramIndex Sampler::getUsedProgram(const int startIndex,
                                          const bool up) const
{
    auto res = startIndex;

    if (up)
    {
        for (int i = startIndex + 1; i < Mpc2000XlSpecs::MAX_PROGRAM_COUNT; i++)
        {
            if (programs[i]->isUsed())
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
            if (programs[i]->isUsed())
            {
                res = i;
                break;
            }
        }
    }

    return ProgramIndex(res);
}

void Sampler::setPlayX(const int i)
{
    playXMode = std::clamp(i, 0, 4);
}

int Sampler::getPlayX() const
{
    return playXMode;
}

bool Sampler::isSoundNameOccupied(const std::string &name) const
{
    for (auto &s : sounds)
    {
        if (StrUtil::eqIgnoreCase(StrUtil::trim(s->getName()),
                                  StrUtil::trim(name)))
        {
            return true;
        }
    }

    return false;
}

std::vector<std::pair<std::shared_ptr<Sound>, int>>
Sampler::getSoundsSortedByName()
{
    std::vector<std::pair<std::shared_ptr<Sound>, int>> result;

    for (int i = 0; i < sounds.size(); i++)
    {
        result.push_back({sounds[i], i});
    }

    std::sort(result.begin(), result.end(),
              [](const std::pair<std::shared_ptr<Sound>, int> &s1,
                 const std::pair<std::shared_ptr<Sound>, int> &s2) -> bool
              {
                  return s1.first->getName() < s2.first->getName();
              });

    return result;
}

std::vector<std::pair<std::shared_ptr<Sound>, int>>
Sampler::getSoundsSortedBySize()
{
    std::vector<std::pair<std::shared_ptr<Sound>, int>> result;

    for (int i = 0; i < sounds.size(); i++)
    {
        result.push_back({sounds[i], i});
    }

    std::sort(result.begin(), result.end(),
              [](const std::pair<std::shared_ptr<Sound>, int> &s1,
                 const std::pair<std::shared_ptr<Sound>, int> &s2) -> bool
              {
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
    if (soundSortingType == 1)
    {
        return getSoundsSortedByName();
    }

    return getSoundsSortedBySize();
}
