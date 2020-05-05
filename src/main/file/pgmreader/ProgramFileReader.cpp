#include <file/pgmreader/ProgramFileReader.hpp>

#include <disk/MpcFile.hpp>
#include <file/pgmreader/PRMixer.hpp>
#include <file/pgmreader/PRPads.hpp>
#include <file/pgmreader/PgmAllNoteParameters.hpp>
#include <file/pgmreader/PgmHeader.hpp>
#include <file/pgmreader/ProgramName.hpp>
#include <file/pgmreader/PRSlider.hpp>
#include <file/pgmreader/SoundNames.hpp>

using namespace mpc::file::pgmreader;
using namespace std;

ProgramFileReader::ProgramFileReader(mpc::disk::MpcFile* f) 
{
	this->programFile = dynamic_pointer_cast<moduru::file::File>(f->getFsNode().lock());
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

std::vector<char> ProgramFileReader::readProgramFileArray()
{
	std::vector<char> res;
	programFile.lock()->getData(&res);
	return res;
}

PgmHeader* ProgramFileReader::getHeader()
{
    return pgmHeader;
}

SoundNames* ProgramFileReader::getSampleNames()
{
    return sampleNames;
}

PgmAllNoteParameters* ProgramFileReader::getAllNoteParameters()
{
    return midiNotes;
}

Mixer* ProgramFileReader::getMixer()
{
    return mixer;
}

Pads* ProgramFileReader::getPads()
{
    return pads;
}

ProgramName* ProgramFileReader::getProgramName()
{
    return programName;
}

Slider* ProgramFileReader::getSlider()
{
    return slider;
}

