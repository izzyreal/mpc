#include <file/sndwriter/SndHeaderWriter.hpp>
#include <file/ByteUtil.hpp>

using namespace mpc::file::sndwriter;
using namespace std;

SndHeaderWriter::SndHeaderWriter(SndWriter* sndWriter) 
{
    headerArray = std::vector<char>(42);
    setFirstTwoBytes();
}

std::vector<char> SndHeaderWriter::getHeaderArray()
{
    return headerArray;
}

void SndHeaderWriter::setFirstTwoBytes()
{
    headerArray[0] = 1;
    headerArray[1] = 4;
}

void SndHeaderWriter::setName(string s)
{
	std::vector<char> nameArray = std::vector<char>(16);
	for (int i = 0; i < s.length(); i++)
		nameArray[i] = s[i];

	for (int i = s.length(); i < nameArray.size(); i++)
		nameArray[i] = 32;

	for (int i = 0; i < nameArray.size(); i++)
		headerArray[i + 2] = nameArray[i];

}

void SndHeaderWriter::setLevel(int i)
{
    headerArray[19] = static_cast< int8_t >(i);
}

void SndHeaderWriter::setTune(int i)
{
    headerArray[20] = static_cast< int8_t >(i);
}

void SndHeaderWriter::setMono(bool b)
{
    headerArray[21] = static_cast< int8_t >((b ? 0 : 1));
}

void SndHeaderWriter::setStart(int i)
{
    putLE(22, i);
}

void SndHeaderWriter::setEnd(int i)
{
    putLE(26, i);
}

void SndHeaderWriter::setFrameCount(int i)
{
    putLE(30, i);
}

void SndHeaderWriter::setLoopLength(int i)
{
    putLE(34, i);
}

void SndHeaderWriter::setLoopEnabled(bool b)
{
    headerArray[38] = (b ? 1 : 0);
}

void SndHeaderWriter::setBeatCount(int i)
{
    headerArray[39] = i;
}

void SndHeaderWriter::setSampleRate(int i)
{
	auto shortBytes = moduru::file::ByteUtil::short2bytes(i - 65536);
	// figure out samplerate offset in header and write 2 bytes
}

void SndHeaderWriter::putLE(int offset, int value)
{
	auto ba = moduru::file::ByteUtil::uint2bytes(value);
	for (auto j = 0; j < ba.size(); j++)
		headerArray[j + offset] = ba[j];
}
