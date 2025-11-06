#pragma once

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
        std::shared_ptr<mpc::engine::StereoMixer>
        getStereoMixerChannel(int noteIndex) const;
        std::shared_ptr<mpc::engine::IndivFxMixer>
        getIndivFxMixerChannel(int noteIndex) const;

        int getPadIndexFromNote(int note) const;

    private:
        Sampler *sampler = nullptr;
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
        mpc::sampler::PgmSlider *getSlider() const;
        void setNoteParameters(int i, NoteParameters *noteParameters);
        int getMidiProgramChange() const;
        void setMidiProgramChange(int i);
        void initPadAssign() const;
        int getNoteFromPad(int i) const;
        std::vector<int> getPadIndicesFromNote(const int note) const;

    public:
        Program(mpc::Mpc &mpc, mpc::sampler::Sampler *samplerToUse);
        ~Program();
    };
} // namespace mpc::sampler
