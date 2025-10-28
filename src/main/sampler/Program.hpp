#pragma once

#include <sampler/NoteParameters.hpp>
#include <sampler/PgmSlider.hpp>

#include <memory>
#include <optional>
#include <unordered_map>
#include <array>
#include <vector>
#include <string>
#include <utility> // for std::pair

namespace mpc::engine
{
    class StereoMixer;
    class IndivFxMixer;
}

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

        enum class NoteEventSource : uint8_t
        {
            PHYSICAL = 0,
            SEQUENCED = 1,
            MIDI = 2,
            GENERATED = 3,
            COUNT
        };

        bool isPadPressedBySource(int padIndex, NoteEventSource source);
        int getPressedPadAfterTouchOrVelocity(int padIndex);
        bool isPadRegisteredAsPressed(int padIndex) const;
        bool isAnyPadRegisteredAsPressed() const;
        void clearActiveNoteRegistry();

        // --- Registration (MIDI-aware)
        void registerMidiNoteOn(int midiChannel, int noteNumber, int velocity,
                                int programPadIndex, NoteEventSource source);
        void registerMidiNoteOff(int midiChannel, int noteNumber,
                                 int programPadIndex, NoteEventSource source);
        void registerMidiNoteAfterTouch(int midiChannel, int noteNumber,
                                        int afterTouch, int programPadIndex);

        // --- Registration (non-MIDI)
        void registerNonMidiNoteOn(int noteNumber, int velocity,
                                   int programPadIndex, NoteEventSource source);
        void registerNonMidiNoteOff(int noteNumber, int programPadIndex,
                                    NoteEventSource source);
        void registerNonMidiNoteAfterTouch(int noteNumber, int afterTouch,
                                           int programPadIndex);

    private:
        Sampler *sampler = nullptr;
        std::string name;
        std::vector<NoteParameters *> noteParameters;
        std::vector<Pad *> pads;
        PgmSlider *slider = nullptr;
        int midiProgramChange = 0;

        void init();

        inline constexpr size_t sourceIndex(NoteEventSource s) {
            return static_cast<size_t>(s);
        }

        struct ActiveNoteState {
            int totalCount = 0;
            std::array<int, static_cast<size_t>(NoteEventSource::COUNT)> sourceCount{};
            int velocity = 0;
            std::optional<int> mostRecentAftertouch = std::nullopt;
            int note = -1;
            int padIndex = -1;
        };

        inline void updateState(ActiveNoteState &state, int velocity, std::optional<int> afterTouch) {
            if (velocity >= 0)
                state.velocity = velocity;
            if (afterTouch.has_value())
                state.mostRecentAftertouch = afterTouch;
        }

        inline void incrementState(ActiveNoteState &s, NoteEventSource src, int velocity)
        {
            s.totalCount++;
            s.sourceCount[sourceIndex(src)]++;
            s.velocity = velocity;
            s.mostRecentAftertouch.reset();
        }

        inline void decrementState(ActiveNoteState &s, NoteEventSource src)
        {
            if (s.totalCount > 0)
                s.totalCount--;
            auto &count = s.sourceCount[sourceIndex(src)];
            if (count > 0)
                count--;
            if (s.totalCount == 0)
            {
                s.sourceCount.fill(0);
                s.mostRecentAftertouch.reset();
                s.padIndex = -1;
                s.note = -1;
            }
        }

        std::array<ActiveNoteState, 128> noteRegistry{};
        std::array<ActiveNoteState, 64> padRegistry{};

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

        Program(mpc::Mpc &mpc, mpc::sampler::Sampler *samplerToUse);
        ~Program();
    };
} // namespace mpc::sampler

