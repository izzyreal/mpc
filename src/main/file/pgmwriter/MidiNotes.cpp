#include "file/pgmwriter/MidiNotes.hpp"

#include "sampler/NoteParameters.hpp"
#include "sampler/Program.hpp"

using namespace mpc::file::pgmwriter;

MidiNotes::MidiNotes(sampler::Program *program,
                     const std::vector<int> &snConvTable)
{
    midiNotesArray = std::vector<char>(1601);

    for (int i = 0; i < 64; i++)
    {
        auto nn = program->getNoteParameters(i + 35);

        if (nn->getSoundIndex() == -1)
        {
            setSampleSelect(i, 255);
        }
        else
        {
            setSampleSelect(i, snConvTable[nn->getSoundIndex()]);
        }

        setSoundGenerationMode(i, nn->getSoundGenerationMode());
        setVelocityRangeLower(i, nn->getVelocityRangeLower());
        setAlsoPlayUse1(i, nn->getOptionalNoteA());
        setVelocityRangeUpper(i, nn->getVelocityRangeUpper());
        setAlsoPlayUse2(i, nn->getOptionalNoteB());
        setVoiceOverlapMode(i, nn->getVoiceOverlapMode());
        setMuteAssign1(i,
                       nn->getMuteAssignA() == 34 ? 0 : nn->getMuteAssignA());
        setMuteAssign2(i,
                       nn->getMuteAssignB() == 34 ? 0 : nn->getMuteAssignB());
        setTune(i, static_cast<int16_t>(nn->getTune()));
        setAttack(i, nn->getAttack());
        setDecay(i, nn->getDecay());
        setDecayMode(i, nn->getDecayMode());
        setCutoff(i, nn->getFilterFrequency());
        setResonance(i, nn->getFilterResonance());
        setVelEnvToFiltAtt(i, nn->getFilterAttack());
        setVelEnvToFiltDec(i, nn->getFilterDecay());
        setVelEnvToFiltAmt(i, nn->getFilterEnvelopeAmount());
        setVelocityToLevel(i, nn->getVeloToLevel());
        setVelocityToAttack(i, nn->getVelocityToAttack());
        setVelocityToStart(i, nn->getVelocityToStart());
        setVelocityToCutoff(i, nn->getVelocityToFilterFrequency());
        setSliderParameter(i, nn->getSliderParameterNumber());
        setVelocityToPitch(i, nn->getVelocityToPitch());
    }

    midiNotesArray[1600] = 6;
}

void MidiNotes::setSampleSelect(int midiNote, int sampleNumber)
{
    char sampleSelect = sampleNumber;
    midiNotesArray[midiNote * 25 + 0] = sampleSelect;
}

void MidiNotes::setSoundGenerationMode(int midiNote, int soundGenerationMode)
{
    midiNotesArray[midiNote * 25 + 1] = soundGenerationMode;
}

void MidiNotes::setVelocityRangeLower(int midiNote, int velocityRangeLower)
{
    midiNotesArray[midiNote * 25 + 2] = velocityRangeLower;
}

void MidiNotes::setAlsoPlayUse1(int midiNote, int alsoPlayUse1)
{
    midiNotesArray[midiNote * 25 + 3] = alsoPlayUse1;
}

void MidiNotes::setVelocityRangeUpper(int midiNote, int velocityRangeUpper)
{
    midiNotesArray[midiNote * 25 + 4] = velocityRangeUpper;
}

void MidiNotes::setAlsoPlayUse2(int midiNote, int alsoPlayUse2)
{
    midiNotesArray[midiNote * 25 + 5] = alsoPlayUse2;
}

void MidiNotes::setVoiceOverlapMode(
    int midiNote, const sampler::VoiceOverlapMode voiceOverlapMode)
{
    midiNotesArray[midiNote * 25 + 6] = static_cast<int>(voiceOverlapMode);
}

void MidiNotes::setMuteAssign1(int midiNote, int muteAssign1)
{
    midiNotesArray[midiNote * 25 + 7] = muteAssign1;
}

void MidiNotes::setMuteAssign2(int midiNote, int muteAssign2)
{
    midiNotesArray[midiNote * 25 + 8] = muteAssign2;
}

void MidiNotes::setTune(int midiNote, int16_t tune)
{
    auto startPos = midiNote * 25 + 9;
    setShort(midiNotesArray, startPos, tune);
}

void MidiNotes::setAttack(int midiNote, int attack)
{
    midiNotesArray[midiNote * 25 + 11] = attack;
}

void MidiNotes::setDecay(int midiNote, int decay)
{
    midiNotesArray[midiNote * 25 + 12] = decay;
}

void MidiNotes::setDecayMode(int midiNote, int decayMode)
{
    midiNotesArray[midiNote * 25 + 13] = decayMode;
}

void MidiNotes::setCutoff(int midiNote, int cutoff)
{
    midiNotesArray[midiNote * 25 + 14] = cutoff;
}

void MidiNotes::setResonance(int midiNote, int resonance)
{
    midiNotesArray[midiNote * 25 + 15] = resonance;
}

void MidiNotes::setVelEnvToFiltAtt(int midiNote, int velEnvToFiltAtt)
{
    midiNotesArray[midiNote * 25 + 16] = velEnvToFiltAtt;
}

void MidiNotes::setVelEnvToFiltDec(int midiNote, int velEnvToFiltDec)
{
    midiNotesArray[midiNote * 25 + 17] = velEnvToFiltDec;
}

void MidiNotes::setVelEnvToFiltAmt(int midiNote, int velEnvToFiltAmt)
{
    midiNotesArray[midiNote * 25 + 18] = velEnvToFiltAmt;
}

void MidiNotes::setVelocityToLevel(int midiNote, int velocityToLevel)
{
    midiNotesArray[midiNote * 25 + 19] = velocityToLevel;
}

void MidiNotes::setVelocityToAttack(int midiNote, int velocityToAttack)
{
    midiNotesArray[midiNote * 25 + 20] = velocityToAttack;
}

void MidiNotes::setVelocityToStart(int midiNote, int velocityToStart)
{
    midiNotesArray[midiNote * 25 + 21] = velocityToStart;
}

void MidiNotes::setVelocityToCutoff(int midiNote, int velocityToCutoff)
{
    midiNotesArray[midiNote * 25 + 22] = velocityToCutoff;
}

void MidiNotes::setSliderParameter(int midiNote, int sliderParameter)
{
    midiNotesArray[midiNote * 25 + 23] = sliderParameter;
}

void MidiNotes::setVelocityToPitch(int midiNote, int velocityToPitch)
{
    midiNotesArray[midiNote * 25 + 24] = velocityToPitch;
}

void MidiNotes::setShort(std::vector<char> &ca, int offset, int16_t s) const
{
    ca[offset] = s;
    ca[offset + 1] = s >> 8;
}
