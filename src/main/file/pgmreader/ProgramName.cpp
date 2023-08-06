#include <file/pgmreader/ProgramName.hpp>

#include <file/pgmreader/ProgramFileReader.hpp>
#include <file/pgmreader/SoundNames.hpp>

#include <Util.hpp>

using namespace mpc::file::pgmreader;

ProgramName::ProgramName(ProgramFileReader* programFile) 
{
    this->programFile = programFile;
}

int ProgramName::getSampleNamesSize()
{
	sampleNamesSize = programFile->getSampleNames()->getSampleNamesSize();
	return sampleNamesSize;
}

int ProgramName::getProgramNameStart()
{
    auto programNameStart = 4 + getSampleNamesSize() + 2;
    return programNameStart;
}

int ProgramName::getProgramNameEnd()
{
    auto programNameEnd = 4 + getSampleNamesSize() + 2+ 17;
    return programNameEnd;
}

std::vector<char> ProgramName::getProgramNameArray()
{
    auto pgmFileArray = programFile->readProgramFileArray();
    programNameArray = Util::vecCopyOfRange(pgmFileArray, getProgramNameStart(), getProgramNameEnd());
    return programNameArray;
}

std::string ProgramName::getProgramNameASCII()
{
	std::string programNameASCII;
	auto bytes = getProgramNameArray();
	for (char c : bytes) {
		if (c == 0x00) break;
		programNameASCII.push_back(c);
	}
	if (programNameASCII.length() > 16)
		programNameASCII = programNameASCII.substr(0, 16);
	return programNameASCII;
}
