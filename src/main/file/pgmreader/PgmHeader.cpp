#include <file/pgmreader/PgmHeader.hpp>

#include <file/pgmreader/ProgramFileReader.hpp>

#include <Util.hpp>

using namespace mpc::file::pgmreader;

PgmHeader::PgmHeader(ProgramFileReader* programFile)
{
    auto pgmFileArray = programFile->readProgramFileArray();
	headerArray = Util::vecCopyOfRange(pgmFileArray, 0, 4);
}

std::vector<char> PgmHeader::getHeaderArray()
{
    return headerArray;
}

bool PgmHeader::verifyFirstTwoBytes()
{
	auto verifyFirstTwoBytes = false;
	int i = headerArray[0];
	int j = headerArray[1];
	if (i == 7 && j == 4) {
		verifyFirstTwoBytes = true;
	}
	return verifyFirstTwoBytes;
}

int PgmHeader::getNumberOfSamples()
{
    auto numberOfSamples = headerArray[2];
    return numberOfSamples;
}
