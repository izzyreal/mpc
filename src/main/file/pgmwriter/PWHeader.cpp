#include <file/pgmwriter/PWHeader.hpp>

using namespace mpc::file::pgmwriter;

PWHeader::PWHeader(int numberOfSamples) 
{
    headerArray = std::vector<char>(4);
    writeFirstTwoBytes();
    setNumberOfSamples(numberOfSamples);
    headerArray[3] = 0;
}

void PWHeader::writeFirstTwoBytes()
{
	headerArray[0] = 7;
	headerArray[1] = 4;
}

void PWHeader::setNumberOfSamples(int numberOfSamples)
{
    headerArray[2] = numberOfSamples;
}

std::vector<char> PWHeader::getHeaderArray()
{
	return headerArray;
}
