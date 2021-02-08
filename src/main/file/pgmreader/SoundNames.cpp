#include <file/pgmreader/SoundNames.hpp>

#include <file/pgmreader/PgmHeader.hpp>
#include <file/pgmreader/ProgramFileReader.hpp>

#include <VecUtil.hpp>

using namespace moduru;
using namespace mpc::file::pgmreader;
using namespace std;

SoundNames::SoundNames(ProgramFileReader* programFile) 
{
    this->programFile = programFile;
}

int SoundNames::getSampleNamesSize()
{
	int sampleNamesSize = (programFile->getHeader()->getNumberOfSamples()) * 17;
	return sampleNamesSize;
}

vector<char> SoundNames::getSampleNamesArray()
{
	auto sampleNamesSize = getSampleNamesSize();
    auto pgmFileArray = programFile->readProgramFileArray();
	sampleNamesArray = VecUtil::CopyOfRange(pgmFileArray, 4, (4 + sampleNamesSize + 2));
	return sampleNamesArray;
}

string SoundNames::getSampleName(int sampleNumber)
{
	string sampleNameString = "";
	auto h = programFile->getHeader();
	if (sampleNumber < h->getNumberOfSamples()) {
		sampleNamesArray = getSampleNamesArray();
		auto sampleName = VecUtil::CopyOfRange(sampleNamesArray, (sampleNumber * 17), ((sampleNumber * 17) + 16));
		for (char c : sampleName) {
			if (c == 0x00) break;
			sampleNameString.push_back(c);
		}
	}
	else {
		sampleNameString = "OFF";
	}
	return sampleNameString;
}
