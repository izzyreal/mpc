#pragma once

#include <sampler/Program.hpp>
#include <sampler/Sound.hpp>

#include <memory>
#include <string>

namespace mpc::engine {
class Drum;
}

namespace mpc::sampler
{

const int16_t PLAYX_SOUND = -4;
const int16_t PREVIEW_SOUND = -3;
const int16_t CLICK_SOUND = -2;

// Not sure if we actually need this.
// We should investigate if PreviewSoundPlayer::mpcNoteOn or Drum::mpcNoteOn ever
// receive calls with `soundIndex` == -1.
const int16_t NO_SOUND = -1;

class Sound;
class Program;

class Sampler final
{
public:
    Sampler(mpc::Mpc&);

    std::shared_ptr<mpc::sampler::Sound> getPlayXSound();
    std::shared_ptr<mpc::sampler::Sound> getClickSound();
    std::shared_ptr<mpc::sampler::Sound> getSound(int index);

    std::shared_ptr<Sound> getPreviewSound();
    std::shared_ptr<Program> getProgram(int index);

    void nudgeSoundIndex(bool up);
    void setSoundIndex(int i);
    short getSoundIndex();
    std::shared_ptr<Sound> getSound();
    std::string getPreviousScreenName();
    void setPreviousScreenName(std::string s);
    bool isSoundNameOccupied(const std::string&);

    void finishBasicVoice();
    void init();
    void playMetronome(unsigned int velocity, int framePos);
    void playPreviewSample(int start, int end, int loopTo);
    int getProgramCount();

    /**
     * The `programs` collection is always of size 24, where a nullptr element
     * indicates a free slot. This method creates a new program at the first
     * available slot. A nullptr is returned if all slots are occupied.
     */
    std::weak_ptr<Program> createNewProgramAddFirstAvailableSlot();

    std::weak_ptr<Program> addProgram(int i);

    void deleteProgram(std::weak_ptr<Program> program);

    /**
     * This method deletes all programs and sets all DRUMs to program
     * index 0.
     *
     * It optionally creates a new program. Be aware that the MPC2000XL
     * always has a valid program in memory, so if createDefaultProgram
     * is false, the caller must make sure there is at least 1 valid
     * program created after the invocation.
     */
    void deleteAllPrograms(bool createDefaultProgram);

    void repairProgramReferences();
    std::vector<std::shared_ptr<Sound>>& getSounds();
    std::shared_ptr<Sound> addSound(const std::string screenToGoToIfSoundDirectoryIsFull);
    std::shared_ptr<Sound> addSound(const int sampleRate, const std::string screenToGoToIfSoundDirectoryIsFull);
    int getSoundCount();
    std::string getSoundName(int i);
    void replaceSound(int index, std::shared_ptr<Sound>& newSound);

    std::string getPadName(int i);
    std::vector<std::weak_ptr<Program>> getPrograms();
    void deleteSound(int deleteSoundIndex);
    void deleteSound(const std::shared_ptr<Sound>& sound);
    void deleteSoundWithoutRepairingPrograms(std::shared_ptr<Sound> sound);
    void trimSample(int sampleNumber, int start, int end);
    void deleteSection(const unsigned int sampleNumber, const unsigned int start, const unsigned int end);
    void switchToNextSoundSortType();
    std::string getSoundSortingTypeName();
    void deleteAllSamples();

    void stopAllVoices(int frameOffset);
    void playX();
    int getFreeSampleSpace();
    std::string addOrIncreaseNumber(std::string s);
    Pad* getLastPad(Program* program);
    NoteParameters* getLastNp(Program* program);
    int getUnusedSampleCount();
    void purge();
    void mergeToStereo(std::shared_ptr<const std::vector<float>> sourceLeft,
                       std::shared_ptr<const std::vector<float>> sourceRight,
                       std::shared_ptr<std::vector<float>> dest);
    void setDrumBusProgramIndex(int busNumber, int programIndex);
    int getDrumBusProgramIndex(int busNumber);
    int getUsedProgram(int startIndex, bool up);
    int checkExists(std::string soundName);
    void selectPreviousSound();
    void selectNextSound();
    std::vector<std::shared_ptr<mpc::engine::StereoMixer>>& getDrumStereoMixerChannels(int i);
    std::vector<std::shared_ptr<mpc::engine::IndivFxMixer>>& getDrumIndivFxMixerChannels(int i);
    std::weak_ptr<Sound> copySound(std::weak_ptr<Sound> source, const std::string screenToGoToIfSoundDirectoryIsFull);
    void copyProgram(const int sourceIndex, const int destIndex);
    std::vector<int>* getInitMasterPadAssign();
    std::vector<int>* getMasterPadAssign();
    void setMasterPadAssign(std::vector<int> v);
    void setPlayX(int i);
    int getPlayX();

    static std::vector<float> mergeToStereo(const std::vector<float> &fa0, const std::vector<float> &fa1);
    static void process12Bit(std::vector<float> &data);
    static void process8Bit(std::vector<float> &data);
    static void resample(std::shared_ptr<const std::vector<float>> data, int sourceRate, std::shared_ptr<Sound> destSnd);
    static std::vector<float> resampleSingleChannel(std::vector<float>& input, int sourceRate, int destRate);
    std::vector<std::pair<std::shared_ptr<Sound>, int>> getSortedSounds();

private:
    int soundIndex = 0;
    int playXMode = 0;
    std::string previousScreenName;

    mpc::Mpc& mpc;
    std::vector<int> initMasterPadAssign;
    std::vector<int> masterPadAssign;

    std::vector<std::shared_ptr<Sound>> sounds;
    std::vector<std::shared_ptr<Program>> programs = std::vector<std::shared_ptr<Program>>(24);
    unsigned char soundSortingType = 0;
    std::vector<std::string> padNames;
    std::vector<std::string> abcd = std::vector<std::string>{ "A", "B", "C", "D" };
    std::shared_ptr<Sound> clickSound;
    std::vector<std::string> sortNames = std::vector<std::string>{ "MEMORY", "NAME", "SIZE" };
    std::vector<std::shared_ptr<Sound>> getUsedSounds();
    int getLastInt(std::string s);
    std::string addOrIncreaseNumber2(std::string s);
    void trimSample(std::weak_ptr<Sound>, int start, int end);
    std::vector<std::pair<std::shared_ptr<Sound>, int>> getSoundsSortedByName();
    std::vector<std::pair<std::shared_ptr<Sound>, int>> getSoundsSortedBySize();
};
}
