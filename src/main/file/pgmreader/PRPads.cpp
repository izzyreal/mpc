#include <file/pgmreader/PRPads.hpp>

#include <file/pgmreader/PgmAllNoteParameters.hpp>
#include <file/pgmreader/ProgramFileReader.hpp>
#include <file/pgmreader/SoundNames.hpp>

#include <VecUtil.hpp>

using namespace moduru;
using namespace mpc::file::pgmreader;
using namespace std;

Pads::Pads(ProgramFileReader* programFile) 
{
    this->programFile = programFile;
}

int Pads::getSampleNamesSize()
{
	sampleNamesSize = programFile->getSampleNames()->getSampleNamesSize();
	return sampleNamesSize;
}

int Pads::getPadsStart()
{
	auto padsStart = 4 + getSampleNamesSize() + 2 + 17 + 9 + 6 + 1601 + 387;
	return padsStart;
}

int Pads::getPadsEnd()
{
	auto padsEnd = 4 + getSampleNamesSize() + 2 + 17 + 9 + 6 + 1601 + 387 + 264;
	return padsEnd;
}

vector<char> Pads::getPadsArray()
{
    auto pgmFileArray = programFile->readProgramFileArray();
	padsArray = VecUtil::CopyOfRange(pgmFileArray, getPadsStart(), getPadsEnd());
	return padsArray;
}

int Pads::getNote(int pad)
{
	int padMidiNote = getPadsArray()[pad];
	return padMidiNote;
}

int Pads::getPadSampleSelect(int pad)
{
	auto midiNote = getNote(pad);
	auto m = programFile->getAllNoteParameters();
	auto padSampleSelect = m->getSampleSelect(midiNote);
	return padSampleSelect;
}

string Pads::getPadSampleName(int pad)
{
	string sampleName;
	auto midiNote = getNote(pad);
	auto m = programFile->getAllNoteParameters();
	auto sampleNumber = m->getSampleSelect(midiNote);
	auto s = programFile->getSampleNames();
	sampleName = s->getSampleName(sampleNumber);
	return sampleName;
}
