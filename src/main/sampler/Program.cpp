#include "sampler/Program.hpp"

#include "IntTypes.hpp"
#include "sampler/Sampler.hpp"
#include "sampler/Pad.hpp"

#include "engine/StereoMixer.hpp"
#include "engine/IndivFxMixer.hpp"

#include "MpcSpecs.hpp"
#include "Mpc.hpp"

using namespace mpc::sampler;
using namespace mpc::engine;

Program::Program(
    Mpc &mpc, Sampler *samplerToUse,
    const std::function<performance::Program()> &getSnapshot,
    const std::function<void(performance::PerformanceMessage &&)> &dispatch)
    : sampler(samplerToUse), getSnapshot(getSnapshot), dispatch(dispatch)
{
    for (int i = 0; i < Mpc2000XlSpecs::PROGRAM_PAD_COUNT; i++)
    {
        auto getNoteParametersSnapshot =
            [this, drumNoteNumber = DrumNoteNumber(i + MinDrumNoteNumber)]
        {
            return this->getSnapshot().getNoteParameters(drumNoteNumber);
        };
        auto n = new NoteParameters(
            i,
            [this]
            {
                return index;
            },
            getNoteParametersSnapshot, dispatch);
        noteParameters.push_back(n);
    }

    for (int i = 0; i < Mpc2000XlSpecs::PROGRAM_PAD_COUNT; i++)
    {
        auto p = new Pad(mpc, ProgramPadIndex(i));
        pads.push_back(p);
    }

    slider = new PgmSlider();
}

void Program::init()
{
    midiProgramChange = 1;
}

int Program::getNumberOfSamples() const
{
    auto counter = 0;

    for (int i = 0; i < Mpc2000XlSpecs::PROGRAM_PAD_COUNT; i++)
    {
        if (const auto np = getNoteParameters(i + MinDrumNoteNumber);
            np->getSoundIndex() != -1)
        {
            counter++;
        }
    }

    return counter;
}

void Program::setName(const std::string &s)
{
    name = s;
}

std::string Program::getName()
{
    return name;
}

NoteParameters *Program::getNoteParameters(const int noteNumber) const
{
    if (noteNumber < MinDrumNoteNumber || noteNumber > MaxDrumNoteNumber)
    {
        return nullptr;
    }

    return noteParameters[noteNumber - MinDrumNoteNumber];
}

Pad *Program::getPad(const int i) const
{
    return pads[i];
}

std::shared_ptr<StereoMixer>
Program::getStereoMixerChannel(const int noteIndex) const
{
    return noteParameters[noteIndex]->getStereoMixer();
}

std::shared_ptr<IndivFxMixer>
Program::getIndivFxMixerChannel(const int noteIndex) const
{
    return noteParameters[noteIndex]->getIndivFxMixer();
}

mpc::ProgramPadIndex
Program::getPadIndexFromNote(const DrumNoteNumber note) const
{
    if (note < MinDrumNoteNumber || note > MaxDrumNoteNumber)
    {
        return NoProgramPadIndex;
    }

    for (int8_t i = 0; i < Mpc2000XlSpecs::PROGRAM_PAD_COUNT; i++)
    {
        if (pads[i]->getNote() == note)
        {
            return ProgramPadIndex{i};
        }
    }

    return NoProgramPadIndex;
}

void Program::setIndex(const ProgramIndex programIndex)
{
    index = programIndex;
}

std::vector<NoteParameters *> Program::getNotesParameters()
{
    return noteParameters;
}

PgmSlider *Program::getSlider() const
{
    return slider;
}

void Program::setNoteParameters(const int noteParametersIndex,
                                NoteParameters *noteParametersToUse)
{
    if (noteParameters[noteParametersIndex] != nullptr)
    {
        delete noteParameters[noteParametersIndex];
    }

    noteParameters[noteParametersIndex] = noteParametersToUse;
}

int Program::getMidiProgramChange() const
{
    return midiProgramChange;
}

void Program::setMidiProgramChange(const int i)
{
    midiProgramChange = std::clamp(i, 1, 128);
}

void Program::initPadAssign() const
{
    for (int i = 0; i < Mpc2000XlSpecs::PROGRAM_PAD_COUNT; i++)
    {
        pads[i]->setNote((*sampler->getInitMasterPadAssign())[i]);
    }
}

mpc::DrumNoteNumber Program::getNoteFromPad(const ProgramPadIndex i) const
{
    return pads[i]->getNote();
}

std::vector<mpc::ProgramPadIndex>
Program::getPadIndicesFromNote(const DrumNoteNumber note) const
{
    std::vector<ProgramPadIndex> result;

    for (int i = 0; i < pads.size(); i++)
    {
        if (pads[i]->getNote() == note)
        {
            result.push_back(ProgramPadIndex(i));
        }
    }

    return result;
}

Program::~Program()
{
    delete slider;
    for (const auto &np : noteParameters)
    {
        delete np;
    }
    for (const auto &p : pads)
    {
        delete p;
    }
}
