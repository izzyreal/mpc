#include "sampler/VoiceOverlapMode.hpp"
#include "file/pgmreader/PgmAllNoteParameters.hpp"

#include "file/pgmreader/PRPads.hpp"
#include "file/pgmreader/ProgramFileReader.hpp"
#include "file/pgmreader/SoundNames.hpp"

#include "file/ByteUtil.hpp"

#include "Util.hpp"

using namespace mpc::file::pgmreader;

PgmAllNoteParameters::PgmAllNoteParameters(ProgramFileReader *programFile)
{
    this->programFile = programFile;
}

int PgmAllNoteParameters::getSampleNamesSize()
{
    sampleNamesSize = programFile->getSampleNames()->getSampleNamesSize();
    return sampleNamesSize;
}

int PgmAllNoteParameters::getMidiNotesStart()
{
    auto midiNotesStart = 4 + getSampleNamesSize() + 2 + 17 + 9 + 6;
    return midiNotesStart;
}

int PgmAllNoteParameters::getMidiNotesEnd()
{
    auto midiNotesEnd = 4 + getSampleNamesSize() + 2 + 17 + 9 + 6 + 1601;
    return midiNotesEnd;
}

std::vector<char> PgmAllNoteParameters::getMidiNotesArray()
{
    auto pgmFileArray = programFile->readProgramFileArray();
    midiNotesArray = Util::vecCopyOfRange(pgmFileArray, getMidiNotesStart(),
                                          getMidiNotesEnd());
    return midiNotesArray;
}

int PgmAllNoteParameters::getSampleSelect(int midiNote)
{
    int sampleSelect = getMidiNotesArray()[(midiNote * 25) + 0];
    return sampleSelect;
}

int PgmAllNoteParameters::getSoundGenerationMode(int midiNote)
{
    auto soundGenerationMode = getMidiNotesArray()[(midiNote * 25) + 1];
    return soundGenerationMode;
}

int PgmAllNoteParameters::getVelocityRangeLower(int midiNote)
{
    auto velocityRangeLower = getMidiNotesArray()[(midiNote * 25) + 2];
    return velocityRangeLower;
}

int PgmAllNoteParameters::getAlsoPlayUse1(int midiNote)
{
    auto alsoPlayUse1 = getMidiNotesArray()[(midiNote * 25) + 3];
    return alsoPlayUse1;
}

int PgmAllNoteParameters::getVelocityRangeUpper(int midiNote)
{
    auto velocityRangeUpper = getMidiNotesArray()[(midiNote * 25) + 4];
    return velocityRangeUpper;
}

int PgmAllNoteParameters::getAlsoPlayUse2(int midiNote)
{
    auto alsoPlayUse2 = getMidiNotesArray()[(midiNote * 25) + 5];
    return alsoPlayUse2;
}

mpc::sampler::VoiceOverlapMode
PgmAllNoteParameters::getVoiceOverlapMode(int midiNote)
{
    auto voiceOverlapMode = getMidiNotesArray()[(midiNote * 25) + 6];
    return static_cast<mpc::sampler::VoiceOverlapMode>(voiceOverlapMode);
}

int PgmAllNoteParameters::getMuteAssign1(int midiNote)
{
    auto muteAssign1 = getMidiNotesArray()[(midiNote * 25) + 7];
    return muteAssign1 == 0 ? 34 : muteAssign1;
}

int PgmAllNoteParameters::getMuteAssign2(int midiNote)
{
    auto muteAssign2 = getMidiNotesArray()[(midiNote * 25) + 8];
    return muteAssign2 == 0 ? 34 : muteAssign2;
}

int PgmAllNoteParameters::getTune(int midiNote)
{
    auto startPos = (midiNote * 25) + 9;
    auto endPos = (midiNote * 25) + 11;
    auto tuneBytes =
        Util::vecCopyOfRange(getMidiNotesArray(), startPos, endPos);
    return ByteUtil::bytes2short(tuneBytes);
}

int PgmAllNoteParameters::getAttack(int midiNote)
{
    auto attack = getMidiNotesArray()[(midiNote * 25) + 11];
    return attack;
}

int PgmAllNoteParameters::getDecay(int midiNote)
{
    auto decay = getMidiNotesArray()[(midiNote * 25) + 12];
    return decay;
}

int PgmAllNoteParameters::getDecayMode(int midiNote)
{
    auto decayMode = getMidiNotesArray()[(midiNote * 25) + 13];
    return decayMode;
}

int PgmAllNoteParameters::getCutoff(int midiNote)
{
    auto cutoff = getMidiNotesArray()[(midiNote * 25) + 14];
    return cutoff;
}

int PgmAllNoteParameters::getResonance(int midiNote)
{
    auto resonance = getMidiNotesArray()[(midiNote * 25) + 15];
    return resonance;
}

int PgmAllNoteParameters::getVelEnvToFiltAtt(int midiNote)
{
    auto velEnvToFiltAtt = getMidiNotesArray()[(midiNote * 25) + 16];
    return velEnvToFiltAtt;
}

int PgmAllNoteParameters::getVelEnvToFiltDec(int midiNote)
{
    auto velEnvToFiltDec = getMidiNotesArray()[(midiNote * 25) + 17];
    return velEnvToFiltDec;
}

int PgmAllNoteParameters::getVelEnvToFiltAmt(int midiNote)
{
    auto velEnvToFiltAmt = getMidiNotesArray()[(midiNote * 25) + 18];
    return velEnvToFiltAmt;
}

int PgmAllNoteParameters::getVelocityToLevel(int midiNote)
{
    auto velocityToLevel = getMidiNotesArray()[(midiNote * 25) + 19];
    return velocityToLevel;
}

int PgmAllNoteParameters::getVelocityToAttack(int midiNote)
{
    auto velocityToAttack = getMidiNotesArray()[(midiNote * 25) + 20];
    return velocityToAttack;
}

int PgmAllNoteParameters::getVelocityToStart(int midiNote)
{
    auto velocityToStart = getMidiNotesArray()[(midiNote * 25) + 21];
    return velocityToStart;
}

int PgmAllNoteParameters::getVelocityToCutoff(int midiNote)
{
    auto velocityToCutoff = getMidiNotesArray()[(midiNote * 25) + 22];
    return velocityToCutoff;
}

int PgmAllNoteParameters::getSliderParameter(int midiNote)
{
    auto sliderParameter = getMidiNotesArray()[(midiNote * 25) + 23];
    return sliderParameter;
}

int PgmAllNoteParameters::getVelocityToPitch(int midiNote)
{
    auto velocityToPitch = getMidiNotesArray()[(midiNote * 25) + 24];
    return velocityToPitch;
}
