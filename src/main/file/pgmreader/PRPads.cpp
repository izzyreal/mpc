#include <file/pgmreader/PRPads.hpp>

#include <file/pgmreader/PgmAllNoteParameters.hpp>
#include <file/pgmreader/ProgramFileReader.hpp>
#include <file/pgmreader/SoundNames.hpp>

#include <Util.hpp>

#include <Util.hpp>
using namespace mpc::file::pgmreader;

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

std::vector<char> Pads::getPadsArray()
{
    auto pgmFileArray = programFile->readProgramFileArray();
	padsArray = Util::vecCopyOfRange(pgmFileArray, getPadsStart(), getPadsEnd());
	return padsArray;
}

int Pads::getNote(int pad)
{
	int padMidiNote = getPadsArray()[pad];
	return padMidiNote;
}
