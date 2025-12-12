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
    const ProgramIndex programIndex, Mpc &mpc, Sampler *const samplerToUse,
    GetProgramFn &getSnapshot,
    const std::function<void(performance::PerformanceMessage &&)> &dispatch)
    : dispatch(dispatch), index(programIndex), slider(new PgmSlider()),
      sampler(samplerToUse), getSnapshot(getSnapshot)
{
    name.reserve(Mpc2000XlSpecs::MAX_PROGRAM_NAME_LENGTH);

    for (int i = 0; i < Mpc2000XlSpecs::PROGRAM_PAD_COUNT; i++)
    {
        GetNoteParametersFn getNoteParametersSnapshot(
            [this, programIndex,
             drumNoteNumber = DrumNoteNumber(i + MinDrumNoteNumber)]
            {
                return this->getSnapshot(programIndex)
                    .getNoteParameters(drumNoteNumber);
            });

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
}

bool Program::isUsed() const
{
    return getSnapshot(index).used;
}

void Program::setUsed() const
{
    performance::PerformanceMessage msg;
    auto p = getSnapshot(index);
    p.resetValuesToDefaults();
    performance::SetProgramUsed payload{index};
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

void Program::resetToDefaultValues() const
{
    performance::PerformanceMessage msg;
    auto p = getSnapshot(index);
    p.resetValuesToDefaults();
    performance::UpdateProgramBulk payload{index, p};
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
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

mpc::ProgramIndex Program::getProgramIndex() const
{
    return index;
}

void Program::cloneNoteParameters(const DrumNoteNumber sourceNote,
                                  const Program *dst,
                                  const DrumNoteNumber destNote) const
{
    const auto snapshot = getSnapshot(index).getNoteParameters(sourceNote);

    dispatch(performance::PerformanceMessage(
        performance::UpdateNoteParametersBySnapshot{dst->getProgramIndex(),
                                                    destNote, snapshot}));
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

std::vector<NoteParameters *> &Program::getNotesParameters()
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
    return getSnapshot(index).midiProgramChange;
}

void Program::setMidiProgramChange(const int i) const
{
    performance::PerformanceMessage msg;
    performance::UpdateProgramMidiProgramChange payload{index,
                                                        std::clamp(i, 1, 128)};
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
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
