#include "file/pgmreader/ProgramFileReader.hpp"

#include "disk/MpcFile.hpp"
#include "file/pgmreader/PRMixer.hpp"
#include "file/pgmreader/PRPads.hpp"
#include "file/pgmreader/PgmAllNoteParameters.hpp"
#include "file/pgmreader/PgmHeader.hpp"
#include "file/pgmreader/ProgramName.hpp"
#include "file/pgmreader/PRSlider.hpp"
#include "file/pgmreader/SoundNames.hpp"

using namespace mpc::file::pgmreader;

ProgramFileReader::ProgramFileReader(
    const std::weak_ptr<disk::MpcFile> &_programFile)
    : programFile(_programFile)
{
    pgmHeader = new PgmHeader(this);
    programName = new ProgramName(this);
    sampleNames = new SoundNames(this);
    midiNotes = new PgmAllNoteParameters(this);
    mixer = new Mixer(this);
    slider = new Slider(this);
    pads = new Pads(this);
}

ProgramFileReader::~ProgramFileReader()
{
    delete pgmHeader;
    delete programName;
    delete sampleNames;
    delete midiNotes;
    delete mixer;
    delete slider;
    delete pads;
}

std::vector<char> ProgramFileReader::readProgramFileArray() const
{
    return programFile.lock()->getBytes();
}

PgmHeader *ProgramFileReader::getHeader() const
{
    return pgmHeader;
}

SoundNames *ProgramFileReader::getSampleNames() const
{
    return sampleNames;
}

PgmAllNoteParameters *ProgramFileReader::getAllNoteParameters() const
{
    return midiNotes;
}

Mixer *ProgramFileReader::getMixer() const
{
    return mixer;
}

Pads *ProgramFileReader::getPads() const
{
    return pads;
}

ProgramName *ProgramFileReader::getProgramName() const
{
    return programName;
}

Slider *ProgramFileReader::getSlider() const
{
    return slider;
}
