#pragma once

#include <vector>
#include <memory>

namespace mpc::disk
{
    class MpcFile;
}

namespace mpc::file::pgmreader
{
    class PgmHeader;
    class SoundNames;
    class ProgramName;
    class PgmAllNoteParameters;
    class Mixer;
    class Slider;
    class Pads;
} // namespace mpc::file::pgmreader

namespace mpc::file::pgmreader
{

    class ProgramFileReader
    {

        PgmHeader *pgmHeader;
        SoundNames *sampleNames;
        ProgramName *programName;
        PgmAllNoteParameters *midiNotes;
        Mixer *mixer;
        Slider *slider;
        Pads *pads;
        std::weak_ptr<disk::MpcFile> programFile;

    public:
        std::vector<char> readProgramFileArray() const;

        PgmHeader *getHeader() const;
        SoundNames *getSampleNames() const;
        PgmAllNoteParameters *getAllNoteParameters() const;
        Mixer *getMixer() const;
        Pads *getPads() const;
        ProgramName *getProgramName() const;
        Slider *getSlider() const;

        ProgramFileReader(const std::weak_ptr<disk::MpcFile> &);
        ~ProgramFileReader();
    };
} // namespace mpc::file::pgmreader
