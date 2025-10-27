#pragma once

#include "MpcSpecs.hpp"
#include <sampler/NoteParameters.hpp>
#include <sampler/PgmSlider.hpp>

#include <memory>

namespace mpc::engine
{
    class StereoMixer;
    class IndivFxMixer;
} // namespace mpc::engine

namespace mpc
{
    class Mpc;
}

namespace mpc::sampler
{

    class Pad;

    class Program
    {

    public:
        std::shared_ptr<mpc::engine::StereoMixer>
        getStereoMixerChannel(int noteIndex);
        std::shared_ptr<mpc::engine::IndivFxMixer>
        getIndivFxMixerChannel(int noteIndex);

        int getPadIndexFromNote(int note);

        enum class PadPressSource : uint8_t
        {
            PHYSICAL = 0,
            SEQUENCED = 1,
            MIDI = 2,
            GENERATED = 3,
            COUNT
        };

        bool isPadPressedBySource(int padIndex, PadPressSource source);

        int getPressedPadAfterTouchOrVelocity(int padIndex);

        void registerPadPress(int padIndex, int velocity, PadPressSource source);

        void registerPadRelease(int padIndex, PadPressSource source);

        void registerPadAfterTouch(int padIndex, int afterTouch);

        bool isPadRegisteredAsPressed(int padIndex) const;

        bool isAnyPadRegisteredAsPressed() const;

        void clearPressedPadRegistry();

    private:
        Sampler *sampler = nullptr;
        std::string name;
        std::vector<NoteParameters *> noteParameters;
        std::vector<Pad *> pads;
        PgmSlider *slider = nullptr;
        int midiProgramChange = 0;

        void init();

        inline constexpr size_t sourceIndex(PadPressSource s)
        {
            return static_cast<size_t>(s);
        }

        struct PadPressState
        {
            int totalCount = 0;
            std::array<int, static_cast<size_t>(PadPressSource::COUNT)>
                sourceCount{};

            int velocity = 0;
            // In case of multiple sources, we only remember the most recently received
            // aftertouch for this pad press.
            std::optional<int> mostRecentAftertouch = std::nullopt;
        };

        std::array<PadPressState, Mpc2000XlSpecs::PROGRAM_PAD_COUNT>
            pressedPadRegistry{};

    public:
        int getNumberOfSamples();
        void setName(std::string s);
        std::string getName();
        Pad *getPad(int i);
        std::vector<NoteParameters *> getNotesParameters();
        NoteParameters *getNoteParameters(int note);
        mpc::sampler::PgmSlider *getSlider();
        void setNoteParameters(int i, NoteParameters *noteParameters);
        int getMidiProgramChange();
        void setMidiProgramChange(int i);
        void initPadAssign();
        int getNoteFromPad(int i);
        std::vector<int> getPadIndicesFromNote(const int note);

    public:
        Program(mpc::Mpc &mpc, mpc::sampler::Sampler *samplerToUse);
        ~Program();
    };
} // namespace mpc::sampler
