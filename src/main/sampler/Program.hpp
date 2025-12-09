#pragma once

#include "IntTypes.hpp"
#include "sampler/NoteParameters.hpp"
#include "sampler/PgmSlider.hpp"
#include "performance/PerformanceMessage.hpp"

#include "utils/SmallFn.hpp"

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

    using GetProgramFn = utils::SmallFn<8, performance::Program(ProgramIndex)>;

    class Program
    {
    public:
        Program(ProgramIndex, Mpc &mpc, Sampler *samplerToUse,
                const GetProgramFn &getSnapshot,
                const std::function<void(performance::PerformanceMessage &&)>
                    &dispatch);

        ~Program();

        std::shared_ptr<engine::StereoMixer>
        getStereoMixerChannel(int noteIndex) const;

        std::shared_ptr<engine::IndivFxMixer>
        getIndivFxMixerChannel(int noteIndex) const;

        ProgramPadIndex getPadIndexFromNote(DrumNoteNumber) const;

    private:
        const ProgramIndex index;
        std::string name;
        PgmSlider *const slider;
        Sampler *const sampler;

        const GetProgramFn getSnapshot;
        const std::function<void(performance::PerformanceMessage &&)> dispatch;

        std::vector<NoteParameters *> noteParameters;
        std::vector<Pad *> pads;

    public:
        bool isUsed() const;

        void setUsed() const;

        void resetToDefaultValues() const;

        int getNumberOfSamples() const;

        void setName(const std::string &s);

        std::string getName();

        Pad *getPad(int i) const;

        std::vector<NoteParameters *> &getNotesParameters();

        NoteParameters *getNoteParameters(int noteNumber) const;

        PgmSlider *getSlider() const;

        void setNoteParameters(int noteParametersIndex,
                               NoteParameters *noteParametersToUse);

        int getMidiProgramChange() const;

        void setMidiProgramChange(int i) const;

        void initPadAssign() const;

        DrumNoteNumber getNoteFromPad(ProgramPadIndex) const;

        std::vector<ProgramPadIndex>
            getPadIndicesFromNote(DrumNoteNumber) const;
    };
} // namespace mpc::sampler
