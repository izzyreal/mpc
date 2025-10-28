#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"
#include "sampler/Pad.hpp"

#include "engine/StereoMixer.hpp"
#include "engine/IndivFxMixer.hpp"

#include "Mpc.hpp"
#include "MpcSpecs.hpp"

#include <stdexcept>

using namespace mpc::sampler;
using namespace mpc::engine;

Program::Program(mpc::Mpc &mpc, mpc::sampler::Sampler *samplerToUse)
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

int Program::getNumberOfSamples()
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

void Program::setName(std::string s)
{
    name = s;
}

std::string Program::getName()
{
    return name;
}

NoteParameters *Program::getNoteParameters(int note)
{
    if (note < 35 || note > 98)
    {
        return nullptr;
    }

    return noteParameters[note - 35];
}

Pad *Program::getPad(int i)
{
    return pads[i];
}

std::shared_ptr<StereoMixer> Program::getStereoMixerChannel(int noteIndex)
{
    return std::dynamic_pointer_cast<StereoMixer>(
        noteParameters[noteIndex]->getStereoMixerChannel());
}

std::shared_ptr<IndivFxMixer> Program::getIndivFxMixerChannel(int noteIndex)
{
    return std::dynamic_pointer_cast<IndivFxMixer>(
        noteParameters[noteIndex]->getIndivFxMixerChannel());
}

int Program::getPadIndexFromNote(int note)
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

mpc::sampler::PgmSlider *Program::getSlider()
{
    return slider;
}

void Program::setNoteParameters(int index, NoteParameters *nn)
{
    if (noteParameters[index] != nullptr)
    {
        delete noteParameters[index];
    }

    noteParameters[index] = nn;
}

int Program::getMidiProgramChange()
{
    return midiProgramChange;
}

void Program::setMidiProgramChange(int i)
{
    if (i < 1 || i > 128)
    {
        return;
    }

    midiProgramChange = i;
}

void Program::initPadAssign()
{
    for (int i = 0; i < Mpc2000XlSpecs::PROGRAM_PAD_COUNT; i++)
    {
        pads[i]->setNote((*sampler->getInitMasterPadAssign())[i]);
    }
}

int Program::getNoteFromPad(int i)
{
    return pads[i]->getNote();
}

std::vector<int> Program::getPadIndicesFromNote(int note)
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

bool Program::isPadPressedBySource(int padIndex, NoteEventSource source)
{
    if (padIndex < 0 || padIndex >= static_cast<int>(padRegistry.size()))
        return false;
    const auto &state = padRegistry[padIndex];
    return state.sourceCount[sourceIndex(source)] > 0;
}

int Program::getPressedPadAfterTouchOrVelocity(int padIndex)
{
    if (padIndex < 0 || padIndex >= static_cast<int>(padRegistry.size()))
        throw std::invalid_argument("Invalid pad index");

    const auto &state = padRegistry[padIndex];
    if (state.totalCount == 0)
        throw std::invalid_argument("Queried pad must be pressed");

    return state.mostRecentAftertouch.value_or(state.velocity);
}

bool Program::isPadRegisteredAsPressed(int padIndex) const
{
    if (padIndex < 0 || padIndex >= static_cast<int>(padRegistry.size()))
        return false;
    return padRegistry[padIndex].totalCount > 0;
}

bool Program::isAnyPadRegisteredAsPressed() const
{
    for (const auto &s : padRegistry)
        if (s.totalCount > 0)
            return true;
    return false;
}

void Program::clearActiveNoteRegistry()
{
    for (auto &s : noteRegistry)
    {
        s.totalCount = 0;
        s.sourceCount.fill(0);
        s.mostRecentAftertouch.reset();
        s.velocity = 0;
        s.padIndex = -1;
        s.note = -1;
    }

    for (auto &s : padRegistry)
    {
        s.totalCount = 0;
        s.sourceCount.fill(0);
        s.mostRecentAftertouch.reset();
        s.velocity = 0;
        s.padIndex = -1;
        s.note = -1;
    }
}

void Program::registerMidiNoteOn(int midiChannel, int noteNumber, int velocity,
                                 int programPadIndex, NoteEventSource source)
{
    if (noteNumber >= 0 && noteNumber < 128)
    {
        auto &note = noteRegistry[noteNumber];
        note.note = noteNumber;
        note.padIndex = programPadIndex;
        incrementState(note, source, velocity);
    }

    if (programPadIndex >= 0 && programPadIndex < static_cast<int>(padRegistry.size()))
    {
        auto &pad = padRegistry[programPadIndex];
        pad.padIndex = programPadIndex;
        pad.note = noteNumber;
        incrementState(pad, source, velocity);
    }

    (void)midiChannel;
}

void Program::registerMidiNoteOff(int midiChannel, int noteNumber,
                                  int programPadIndex, NoteEventSource source)
{
    if (noteNumber >= 0 && noteNumber < 128)
        decrementState(noteRegistry[noteNumber], source);

    if (programPadIndex >= 0 && programPadIndex < static_cast<int>(padRegistry.size()))
        decrementState(padRegistry[programPadIndex], source);

    (void)midiChannel;
}

void Program::registerMidiNoteAfterTouch(int midiChannel, int noteNumber,
                                         int afterTouch, int programPadIndex)
{
    if (noteNumber >= 0 && noteNumber < 128)
        noteRegistry[noteNumber].mostRecentAftertouch = afterTouch;

    if (programPadIndex >= 0 && programPadIndex < static_cast<int>(padRegistry.size()))
        padRegistry[programPadIndex].mostRecentAftertouch = afterTouch;

    (void)midiChannel;
}

void Program::registerNonMidiNoteOn(int noteNumber, int velocity,
                                    int programPadIndex, NoteEventSource source)
{
    registerMidiNoteOn(-1, noteNumber, velocity, programPadIndex, source);
}

void Program::registerNonMidiNoteOff(int noteNumber, int programPadIndex,
                                     NoteEventSource source)
{
    registerMidiNoteOff(-1, noteNumber, programPadIndex, source);
}

void Program::registerNonMidiNoteAfterTouch(int noteNumber, int afterTouch,
                                            int programPadIndex)
{
    registerMidiNoteAfterTouch(-1, noteNumber, afterTouch, programPadIndex);
}

