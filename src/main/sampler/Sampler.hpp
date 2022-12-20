#pragma once

#include <mpc/MpcSampler.hpp>

#include <sampler/Program.hpp>
#include <sampler/Sound.hpp>

#include <set>
#include <memory>

namespace ctoot::mpc {
class MpcSound;
class MpcProgram;
class MpcSoundPlayerChannel;
}

namespace mpc::sequencer {
class NoteEvent;
}

namespace mpc::sampler
{
class Sound;
class Program;

class Sampler final
: public virtual ctoot::mpc::MpcSampler
{
    
public:
    Sampler(mpc::Mpc&);

    std::shared_ptr<ctoot::mpc::MpcSound> getMpcPreviewSound() override;
    std::shared_ptr<ctoot::mpc::MpcSound> getPlayXSound() override;
    std::shared_ptr<ctoot::mpc::MpcSound> getClickSound() override;
    std::shared_ptr<ctoot::mpc::MpcSound> getMpcSound(int index) override;
    std::shared_ptr<ctoot::mpc::MpcProgram> getMpcProgram(int programNumber) override;
    
    std::shared_ptr<Sound> getPreviewSound();
    std::shared_ptr<Sound> getSound(int index);
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
    void playMetronome(mpc::sequencer::NoteEvent* event, int framePos);
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
     * Use this method with caution. It brings the MPC2000XL in an invalid
     * state, as there should always be one valid program. Callers of this
     * method have the responsibility to add at least one program after
     * making the call.
     */
    void deleteAllPrograms();

    void repairProgramReferences();
    std::vector<std::shared_ptr<Sound>>& getSounds();
    std::shared_ptr<Sound> addSound();
    std::shared_ptr<Sound> addSound(int sampleRate);
    int getSoundCount();
    std::string getSoundName(int i);
    void replaceSound(int index, std::shared_ptr<Sound>& newSound);

    std::string getPadName(int i);
    std::vector<std::weak_ptr<Program>> getPrograms();
    void deleteSound(int deleteSoundIndex);
    void deleteSound(std::weak_ptr<Sound> sound);
    void deleteSoundWithoutRepairingPrograms(std::shared_ptr<Sound> sound);
    void trimSample(int sampleNumber, int start, int end);
    void deleteSection(const unsigned int sampleNumber, const unsigned int start, const unsigned int end);
    void switchToNextSoundSortType();
    std::string getSoundSortingTypeName();
    void deleteAllSamples();
    std::weak_ptr<Sound> createZone(std::weak_ptr<Sound> source, int start, int end, int endMargin);

    void stopAllVoices(int frameOffset);
    void playX();
    int getFreeSampleSpace();
    std::string addOrIncreaseNumber(std::string s);
    Pad* getLastPad(Program* program);
    NoteParameters* getLastNp(Program* program);
    int getUnusedSampleCount();
    void purge();
    void mergeToStereo(std::vector<float>* sourceLeft, std::vector<float>* sourceRight, std::vector<float>* dest);
    void setDrumBusProgramIndex(int busNumber, int programIndex);
    int getDrumBusProgramIndex(int busNumber);
    ctoot::mpc::MpcSoundPlayerChannel* getDrum(int i);
    int getUsedProgram(int startIndex, bool up);
    int checkExists(std::string soundName);
    void selectPreviousSound();
    void selectNextSound();
    std::vector<std::weak_ptr<ctoot::mpc::MpcStereoMixerChannel>> getDrumStereoMixerChannels(int i);
    std::vector<std::weak_ptr<ctoot::mpc::MpcIndivFxMixerChannel>> getDrumIndivFxMixerChannels(int i);
    std::weak_ptr<Sound> copySound(std::weak_ptr<Sound> source);
    void copyProgram(const int sourceIndex, const int destIndex);
    std::vector<int>* getInitMasterPadAssign();
    std::vector<int>* getMasterPadAssign();
    void setMasterPadAssign(std::vector<int> v);
    void setPlayX(int i);
    int getPlayX();
    
    static std::vector<float> mergeToStereo(std::vector<float> fa0, std::vector<float> fa1);
    static void process12Bit(std::vector<float>* data);
    static void process8Bit(std::vector<float>* data);
    static void resample(std::vector<float>& data, int sourceRate, std::shared_ptr<Sound> destSnd);
    std::vector<std::pair<std::shared_ptr<Sound>, int>> getSortedSounds();
//    short getSortedSoundIndex();
//    void setSortedSoundIndex(short newSortedSoundIndex);

private:
//    static bool compareMemoryIndex(std::weak_ptr<Sound> a, std::weak_ptr<Sound> b);
//    static bool compareName(std::weak_ptr<Sound> a, std::weak_ptr<Sound> b);
//    static bool compareSize(std::weak_ptr<Sound> a, std::weak_ptr<Sound> b);

    int soundIndex = 0;
    int playXMode = 0;
    std::string previousScreenName = "";
    
    mpc::Mpc& mpc;
    std::vector<int> initMasterPadAssign;
    std::vector<int> masterPadAssign;

    std::vector<std::shared_ptr<Sound>> sounds;
    std::vector<std::shared_ptr<Program>> programs = std::vector<std::shared_ptr<Program>>(24);
    unsigned char soundSortingType = 0;
    std::vector<std::string> padNames;
    std::vector<std::string> abcd = std::vector<std::string>{ "A", "B", "C", "D" };
    std::vector<float> clickSample;
    std::shared_ptr<Sound> clickSound;
    std::vector<std::string> sortNames = std::vector<std::string>{ "MEMORY", "NAME", "SIZE" };
    std::vector<std::weak_ptr<Sound>> getUsedSounds();
    int getLastInt(std::string s);
    std::string addOrIncreaseNumber2(std::string s);
    void trimSample(std::weak_ptr<Sound>, int start, int end);
    std::vector<std::pair<std::shared_ptr<Sound>, int>> getSoundsSortedByName();
    std::vector<std::pair<std::shared_ptr<Sound>, int>> getSoundsSortedBySize();
};
}
