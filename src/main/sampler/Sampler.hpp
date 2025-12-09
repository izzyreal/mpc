#pragma once

#include "sampler/Program.hpp"
#include "sampler/Sound.hpp"
#include "MpcSpecs.hpp"
#include "IntTypes.hpp"

#include <memory>
#include <string>

namespace mpc::sampler
{
    constexpr int16_t PLAYX_SOUND = -4;
    constexpr int16_t PREVIEW_SOUND = -3;
    constexpr int16_t CLICK_SOUND = -2;

    // Not sure if we actually need this.
    // We should investigate if this is ever used
    constexpr int16_t NO_SOUND = -1;

    class Sound;
    class Program;

    class Sampler final
    {
    public:
        explicit Sampler(
            Mpc &,
            const GetProgramFn
                &getSnapshot,
            const std::function<void(performance::PerformanceMessage &&)>
                &dispatch);

        std::shared_ptr<Sound> getPlayXSound();
        std::shared_ptr<Sound> getClickSound();
        std::shared_ptr<Sound> getSound(int index);

        std::shared_ptr<Sound> getPreviewSound();
        std::shared_ptr<Program> getProgram(int index);

        void nudgeSoundIndex(bool up);
        void setSoundIndex(int i);
        short getSoundIndex() const;
        std::shared_ptr<Sound> getSound();
        std::string getPreviousScreenName();
        void setPreviousScreenName(const std::string &s);
        bool isSoundNameOccupied(const std::string &) const;

        void init();
        void playMetronome(unsigned int velocity, int framePos) const;
        void playPreviewSample(int start, int end, int loopTo) const;
        int getProgramCount() const;

        /**
         * The `programs` collection is always of size MAX_PROGRAM_COUNT (which
         * is 24 for an MPC2000XL), where a nullptr element indicates a free
         * slot. This method creates a new program at the first available slot.
         * A nullptr is returned if all slots are occupied.
         */
        std::weak_ptr<Program> createNewProgramAddFirstAvailableSlot();

        std::weak_ptr<Program> addProgram(int i);

        void deleteProgram(const std::weak_ptr<Program> &program) const;

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

        std::vector<std::shared_ptr<Sound>> &getSounds();
        std::shared_ptr<Sound> addSound();
        std::shared_ptr<Sound> addSound(int sampleRate);
        int getSoundCount() const;
        std::string getSoundName(int i) const;
        void replaceSound(int index, const std::shared_ptr<Sound> &newSound);

        std::string getPadName(int i);
        std::vector<std::weak_ptr<Program>> getPrograms() const;
        void deleteSound(int deleteSoundIndex);
        void deleteSound(const std::shared_ptr<Sound> &sound);
        void deleteSoundWithoutRepairingPrograms(
            const std::shared_ptr<Sound> &sound);
        void trimSample(int sampleNumber, int start, int end) const;
        void deleteSection(unsigned int sampleNumber, unsigned int start,
                           unsigned int end) const;
        void switchToNextSoundSortType();
        std::string getSoundSortingTypeName() const;
        void deleteAllSamples();

        void stopAllVoices(int frameOffset) const;
        void playX();
        int getFreeSampleSpace() const;
        std::string addOrIncreaseNumber(const std::string &s) const;
        int getUnusedSampleCount() const;
        void purge();

        static void mergeToStereo(
            const std::shared_ptr<const std::vector<float>> &sourceLeft,
            const std::shared_ptr<const std::vector<float>> &sourceRight,
            const std::shared_ptr<std::vector<float>> &dest);

        ProgramIndex getUsedProgram(int startIndex, bool up) const;
        int checkExists(const std::string &soundName) const;
        void selectPreviousSound();
        void selectNextSound();
        std::weak_ptr<Sound> copySound(const std::weak_ptr<Sound> &source);
        void copyProgram(int sourceIndex, int destIndex);
        std::vector<DrumNoteNumber> *getInitMasterPadAssign();
        std::vector<DrumNoteNumber> *getMasterPadAssign();
        void setMasterPadAssign(const std::vector<DrumNoteNumber> &v);
        void setPlayX(int i);
        int getPlayX() const;

        static std::vector<float> mergeToStereo(const std::vector<float> &fa0,
                                                const std::vector<float> &fa1);
        static void process12Bit(std::vector<float> &data);
        static void process8Bit(std::vector<float> &data);
        static void
        resample(const std::shared_ptr<const std::vector<float>> &data,
                 int sourceRate, const std::shared_ptr<Sound> &destSnd);
        std::vector<std::pair<std::shared_ptr<Sound>, int>> getSortedSounds();

    private:
        Mpc &mpc;
        const GetProgramFn getSnapshot;
        const std::function<void(performance::PerformanceMessage &&)> dispatch;
        int soundIndex = 0;
        int playXMode = 0;
        std::string previousScreenName;

        std::vector<DrumNoteNumber> initMasterPadAssign;
        std::vector<DrumNoteNumber> masterPadAssign;

        std::vector<std::shared_ptr<Sound>> sounds;
        std::vector<std::shared_ptr<Program>> programs =
            std::vector<std::shared_ptr<Program>>(
                Mpc2000XlSpecs::MAX_PROGRAM_COUNT);
        unsigned char soundSortingType = 0;
        std::vector<std::string> padNames;
        std::vector<std::string> abcd =
            std::vector<std::string>{"A", "B", "C", "D"};
        std::shared_ptr<Sound> clickSound;
        std::vector<std::string> sortNames =
            std::vector<std::string>{"MEMORY", "NAME", "SIZE"};
        std::vector<std::shared_ptr<Sound>> getUsedSounds() const;

        static int getLastInt(const std::string &s);

        static std::string addOrIncreaseNumber2(const std::string &s);

        static void trimSample(const std::weak_ptr<Sound> &, int start,
                               int end);
        std::vector<std::pair<std::shared_ptr<Sound>, int>>
        getSoundsSortedByName();
        std::vector<std::pair<std::shared_ptr<Sound>, int>>
        getSoundsSortedBySize();
    };
} // namespace mpc::sampler
