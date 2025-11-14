#pragma once

#include "IntTypes.hpp"
#include "sampler/NoteParameters.hpp"
#include "sampler/PgmSlider.hpp"

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
        Program(Mpc &mpc, Sampler *samplerToUse);
        ~Program();

        std::shared_ptr<engine::StereoMixer>
        getStereoMixerChannel(int noteIndex) const;
        std::shared_ptr<engine::IndivFxMixer>
        getIndivFxMixerChannel(int noteIndex) const;

        ProgramPadIndex getPadIndexFromNote(DrumNoteNumber) const;

    private:
        Sampler *const sampler;
        std::string name;
        std::vector<NoteParameters *> noteParameters;
        std::vector<Pad *> pads;
        PgmSlider *slider = nullptr;
        int midiProgramChange = 0;

        void init();

    public:
        int getNumberOfSamples() const;
        void setName(const std::string &s);
        std::string getName();
        Pad *getPad(int i) const;
        std::vector<NoteParameters *> getNotesParameters();
        NoteParameters *getNoteParameters(int note) const;
        PgmSlider *getSlider() const;
        void setNoteParameters(int noteParametersIndex,
                               NoteParameters *noteParametersToUse);
        int getMidiProgramChange() const;
        void setMidiProgramChange(int i);
        void initPadAssign() const;
        DrumNoteNumber getNoteFromPad(ProgramPadIndex) const;
        std::vector<ProgramPadIndex>
            getPadIndicesFromNote(DrumNoteNumber) const;
    };
} // namespace mpc::sampler
