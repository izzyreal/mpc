#include <file/aps/ApsDrumConfiguration.hpp>

#include <file/BitUtil.hpp>
#include <string>

using namespace moduru::file;
using namespace mpc::file::aps;
using namespace std;

ApsDrumConfiguration::ApsDrumConfiguration(vector<char> loadBytes)
{
	program = loadBytes[5];
	receivePgmChange = BitUtil::getBits(loadBytes[2])[7] == '1';
	receiveMidiVolume = BitUtil::getBits(loadBytes[3])[7] == '1';
}

ApsDrumConfiguration::ApsDrumConfiguration(int program, bool recPgmChange, bool recMidiVolume)
{
	saveBytes = vector<char>(12);
	for (int i = 0; i < 9; i++)
		saveBytes[i] = TEMPLATE[i];

	for (int i = 0; i < 3; i++)
		saveBytes[i + 9] = (PADDING[i]);

	saveBytes[1] = program;
	saveBytes[2] = BitUtil::setBits(1, saveBytes[2], recPgmChange);
	saveBytes[3] = BitUtil::setBits(1, saveBytes[3], recMidiVolume);
	saveBytes[5] = program;
}

vector<char> ApsDrumConfiguration::TEMPLATE = vector<char>{ 0, 0, 1, 1, 127, 0, 1, 1, 127 };
vector<char> ApsDrumConfiguration::PADDING = vector<char>{ 64, 0, 6 };

int ApsDrumConfiguration::getProgram()
{
    return program;
}

bool ApsDrumConfiguration::getReceivePgmChange()
{
    return receivePgmChange;
}

bool ApsDrumConfiguration::getReceiveMidiVolume()
{
    return receiveMidiVolume;
}

vector<char> ApsDrumConfiguration::getBytes()
{
    return saveBytes;
}
