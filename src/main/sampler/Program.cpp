#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"
#include "sampler/Pad.hpp"

#include "engine/StereoMixer.hpp"
#include "engine/IndivFxMixer.hpp"

#include "MpcSpecs.hpp"
#include "Mpc.hpp"

#include <stdexcept>

using namespace mpc::sampler;
using namespace mpc::engine;

Program::Program(Mpc &mpc, Sampler *samplerToUse)
{
    sampler = samplerToUse;

    for (int i = 0; i < Mpc2000XlSpecs::PROGRAM_PAD_COUNT; i++)
    {
        auto n = new NoteParameters(i);
        noteParameters.push_back(n);
    }

    for (int i = 0; i < Mpc2000XlSpecs::PROGRAM_PAD_COUNT; i++)
    {
        auto p = new Pad(mpc, i);
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
        auto np = getNoteParameters(i + 35);

        if (np->getSoundIndex() != -1)
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

NoteParameters *Program::getNoteParameters(const int note) const
{
    if (note < 35 || note > 98)
    {
        return nullptr;
    }

    return noteParameters[note - 35];
}

Pad *Program::getPad(const int i) const
{
    return pads[i];
}

std::shared_ptr<StereoMixer>
Program::getStereoMixerChannel(const int noteIndex) const
{
    return std::dynamic_pointer_cast<StereoMixer>(
        noteParameters[noteIndex]->getStereoMixerChannel());
}

std::shared_ptr<IndivFxMixer>
Program::getIndivFxMixerChannel(const int noteIndex) const
{
    return std::dynamic_pointer_cast<IndivFxMixer>(
        noteParameters[noteIndex]->getIndivFxMixerChannel());
}

int Program::getPadIndexFromNote(const int note) const
{
    if (note < 35 || note > 98)
    {
        return -1;
    }

    for (int i = 0; i < Mpc2000XlSpecs::PROGRAM_PAD_COUNT; i++)
    {
        if (pads[i]->getNote() == note)
        {
            return i;
        }
    }

    return -1;
}

std::vector<NoteParameters *> Program::getNotesParameters()
{
    return noteParameters;
}

PgmSlider *Program::getSlider() const
{
    return slider;
}

void Program::setNoteParameters(const int index, NoteParameters *nn)
{
    if (noteParameters[index] != nullptr)
    {
        delete noteParameters[index];
    }

    noteParameters[index] = nn;
}

int Program::getMidiProgramChange() const
{
    return midiProgramChange;
}

void Program::setMidiProgramChange(const int i)
{
    if (i < 1 || i > 128)
    {
        return;
    }

    midiProgramChange = i;
}

void Program::initPadAssign() const
{
    for (int i = 0; i < Mpc2000XlSpecs::PROGRAM_PAD_COUNT; i++)
    {
        pads[i]->setNote((*sampler->getInitMasterPadAssign())[i]);
    }
}

int Program::getNoteFromPad(const int i) const
{
    return pads[i]->getNote();
}

std::vector<int> Program::getPadIndicesFromNote(const int note) const
{
    std::vector<int> result;

    for (int i = 0; i < pads.size(); i++)
    {
        if (pads[i]->getNote() == note)
        {
            result.push_back(i);
        }
    }

    return result;
}

Program::~Program()
{
    delete slider;
    for (auto &np : noteParameters)
    {
        delete np;
    }
    for (auto &p : pads)
    {
        delete p;
    }
}
