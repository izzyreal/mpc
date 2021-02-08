#include <file/pgmreader/ProgramName.hpp>

#include <file/pgmreader/ProgramFileReader.hpp>
#include <file/pgmreader/SoundNames.hpp>

#include <VecUtil.hpp>

using namespace moduru;
using namespace mpc::file::pgmreader;
using namespace std;

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

vector<char> ProgramName::getProgramNameArray()
{
    auto pgmFileArray = programFile->readProgramFileArray();
    programNameArray = VecUtil::CopyOfRange(pgmFileArray, getProgramNameStart(), getProgramNameEnd());
    return programNameArray;
}

string ProgramName::getProgramNameASCII()
{
	string programNameASCII = "";
	auto bytes = getProgramNameArray();
	for (char c : bytes) {
		if (c == 0x00) break;
		programNameASCII.push_back(c);
	}
	if (programNameASCII.length() > 16)
		programNameASCII = programNameASCII.substr(0, 16);
	return programNameASCII;
}

void ProgramName::setProgramNameASCII(string programName)
{
	int j = 0;
	for (int i = getProgramNameStart(); i < getProgramNameEnd(); i++) {
		programFile->readProgramFileArray()[i] = programName[j];
		j++;
	}
}
