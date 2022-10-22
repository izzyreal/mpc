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

    std::weak_ptr<ctoot::mpc::MpcSound> getMpcPreviewSound() override;
    std::weak_ptr<ctoot::mpc::MpcSound> getPlayXSound() override;
    std::weak_ptr<ctoot::mpc::MpcSound> getClickSound() override;
    std::weak_ptr<ctoot::mpc::MpcSound> getMpcSound(int index) override;
    std::weak_ptr<ctoot::mpc::MpcProgram> getMpcProgram(int programNumber) override;
    
    std::weak_ptr<Sound> getPreviewSound();
    std::weak_ptr<Sound> getSound(int index);
    std::weak_ptr<Program> getProgram(int index);
        
    void setSoundIndex(int i);
    int getSoundIndex();
    std::weak_ptr<Sound> getSound();
    std::string getPreviousScreenName();
    void setPreviousScreenName(std::string s);
    bool isSoundNameOccupied(const std::string&);
        
    int getInputLevel();
    void setInputLevel(int i);
    
    void finishBasicVoice();
    void init();
    void playMetronome(mpc::sequencer::NoteEvent* event, int framePos);
    void playPreviewSample(int start, int end, int loopTo, int overlapMode);
    int getProgramCount();
    std::weak_ptr<Program> addProgram();
    std::weak_ptr<Program> addProgram(int i);
    void replaceProgram(std::weak_ptr<Program> p, int index);
    void deleteProgram(std::weak_ptr<Program> program);
    void deleteAllPrograms(bool init);
    void repairProgramReferences();
    std::vector<std::weak_ptr<Sound>> getSounds();
    std::weak_ptr<Sound> addSound();
    std::weak_ptr<Sound> addSound(int sampleRate);
    int getSoundCount();
    std::string getSoundName(int i);
    std::vector<std::string> getSoundNames();
    std::string getPadName(int i);
    std::vector<std::weak_ptr<Program>> getPrograms();
    void deleteSound(int deleteSoundIndex);
    void deleteSound(std::weak_ptr<Sound> sound);
    void trimSample(int sampleNumber, int start, int end);
    void deleteSection(const unsigned int sampleNumber, const unsigned int start, const unsigned int end);
    void sort();
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
    int getNextSoundIndex(int j, bool up);
    void selectPreviousSound();
    void selectNextSound();
    std::vector<std::weak_ptr<ctoot::mpc::MpcStereoMixerChannel>> getDrumStereoMixerChannels(int i);
    std::vector<std::weak_ptr<ctoot::mpc::MpcIndivFxMixerChannel>> getDrumIndivFxMixerChannels(int i);
    std::weak_ptr<Sound> copySound(std::weak_ptr<Sound> source);
    void copyProgram(const int sourceIndex, const int destIndex);
    std::vector<int>* getInitMasterPadAssign();
    std::vector<int>* getMasterPadAssign();
    void setMasterPadAssign(std::vector<int> v);
    std::vector<int>* getAutoChromaticAssign();
    void setPlayX(int i);
    int getPlayX();
    
    static std::vector<float> mergeToStereo(std::vector<float> fa0, std::vector<float> fa1);
    static void process12Bit(std::vector<float>* data);
    static void process8Bit(std::vector<float>* data);
    static void resample(std::vector<float>& data, int sourceRate, std::shared_ptr<Sound> destSnd);
    
private:
    static bool compareMemoryIndex(std::weak_ptr<Sound> a, std::weak_ptr<Sound> b);
    static bool compareName(std::weak_ptr<Sound> a, std::weak_ptr<Sound> b);
    static bool compareSize(std::weak_ptr<Sound> a, std::weak_ptr<Sound> b);

    int inputLevel = 0;
    int soundIndex = 0;
    int playX_ = 0;
    std::string previousScreenName = "";
    
    mpc::Mpc& mpc;
    std::vector<int> initMasterPadAssign;
    std::vector<int> masterPadAssign;
    std::vector<int> autoChromaticAssign;
    
    std::vector<std::shared_ptr<Sound>> sounds;
    std::vector<std::shared_ptr<Program>> programs = std::vector<std::shared_ptr<Program>>(24);
    int soundSortingType = 0;
    std::vector<std::string> padNames;
    std::vector<std::string> abcd = std::vector<std::string>{ "A", "B", "C", "D" };
    std::vector<float> clickSample;
    std::shared_ptr<Sound> clickSound;
    std::vector<std::string> sortNames = std::vector<std::string>{ "MEMORY", "NAME", "SIZE" };
    std::vector<std::weak_ptr<Sound>> getUsedSounds();
    int getLastInt(std::string s);
    std::string addOrIncreaseNumber2(std::string s);
    void trimSample(std::weak_ptr<Sound>, int start, int end);

};
}
