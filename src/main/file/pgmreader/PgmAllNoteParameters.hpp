#pragma once

#include "sampler/VoiceOverlapMode.hpp"
#include <vector>

namespace mpc::file::pgmreader
{
    class ProgramFileReader;

    class PgmAllNoteParameters
    {

    public:
        int sampleNamesSize = 0;

    private:
        std::vector<char> midiNotesArray;

    private:
        ProgramFileReader *programFile;

    public:
        int getSampleNamesSize();
        int getMidiNotesStart();
        int getMidiNotesEnd();
        std::vector<char> getMidiNotesArray();

    public:
        int getSampleSelect(int midiNote);
        int getSoundGenerationMode(int midiNote);
        int getVelocityRangeLower(int midiNote);
        int getAlsoPlayUse1(int midiNote);
        int getVelocityRangeUpper(int midiNote);
        int getAlsoPlayUse2(int midiNote);
        sampler::VoiceOverlapMode getVoiceOverlapMode(int midiNote);
        int getMuteAssign1(int midiNote);
        int getMuteAssign2(int midiNote);
        int getTune(int midiNote);
        int getAttack(int midiNote);
        int getDecay(int midiNote);
        int getDecayMode(int midiNote);
        int getCutoff(int midiNote);
        int getResonance(int midiNote);
        int getVelEnvToFiltAtt(int midiNote);
        int getVelEnvToFiltDec(int midiNote);
        int getVelEnvToFiltAmt(int midiNote);
        int getVelocityToLevel(int midiNote);
        int getVelocityToAttack(int midiNote);
        int getVelocityToStart(int midiNote);
        int getVelocityToCutoff(int midiNote);
        int getSliderParameter(int midiNote);
        int getVelocityToPitch(int midiNote);

        PgmAllNoteParameters(ProgramFileReader *programFile);
    };
} // namespace mpc::file::pgmreader
