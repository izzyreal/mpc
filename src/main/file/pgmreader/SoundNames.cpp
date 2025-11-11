#include "file/pgmreader/SoundNames.hpp"

#include "file/pgmreader/PgmHeader.hpp"
#include "file/pgmreader/ProgramFileReader.hpp"

#include "Util.hpp"

using namespace mpc::file::pgmreader;

SoundNames::SoundNames(ProgramFileReader *programFile)
{
    this->programFile = programFile;
}

int SoundNames::getSampleNamesSize() const
{
    int sampleNamesSize = programFile->getHeader()->getSoundCount() * 17;
    return sampleNamesSize;
}

std::vector<char> SoundNames::getSampleNamesArray()
{
    auto sampleNamesSize = getSampleNamesSize();
    auto pgmFileArray = programFile->readProgramFileArray();
    sampleNamesArray =
        Util::vecCopyOfRange(pgmFileArray, 4, 4 + sampleNamesSize + 2);
    return sampleNamesArray;
}

std::string SoundNames::getSampleName(int sampleNumber)
{
    std::string sampleNameString;
    auto h = programFile->getHeader();
    if (sampleNumber < h->getSoundCount())
    {
        sampleNamesArray = getSampleNamesArray();
        auto sampleName = Util::vecCopyOfRange(
            sampleNamesArray, sampleNumber * 17, sampleNumber * 17 + 16);
        for (char c : sampleName)
        {
            if (c == 0x00)
            {
                break;
            }
            sampleNameString.push_back(c);
        }
    }
    else
    {
        sampleNameString = "OFF";
    }
    return sampleNameString;
}
