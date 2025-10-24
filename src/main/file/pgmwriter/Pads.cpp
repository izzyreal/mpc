#include "file/pgmwriter/Pads.hpp"

#include "sampler/Pad.hpp"
#include "sampler/Program.hpp"

#include "file/ByteUtil.hpp"

using namespace mpc::file::pgmwriter;

Pads::Pads(mpc::sampler::Program* program)
{

	padsArray = std::vector<char>(264);
	for (int i = 0; i < 64; i++) {
		auto nn = program->getPad(i)->getNote();
		if (nn == 34) {
			setPadMidiNote(i, 0);
		}
		else {
			setPadMidiNote(i, nn);
		}
	}
	auto paddingBytes = getFxBoardSettings();
	for (int i = 0; i < 200; i++)
		padsArray[i + 64] = paddingBytes[i];
}
std::string Pads::fxBoardSettings = "02004800D0070000630114081DFC323302323C08050A1414320000020F19000541141E01050000056300F4FF0C0000000000000002004F014F0100424F0100424F01004232006328003C0000D0070000630114081DFC323302323C08050A1414320000020F19000541141E01050000056300F4FF0C0000000000000002004F014F0100424F0100424F01004232006328003C000004000C000000320023003E335A3214000000320023003E335A3214000000320023003E335A3214000000320023003E335A321400";

std::vector<char> Pads::getPadsArray()
{
    return padsArray;
}

void Pads::setPadMidiNote(int pad, int padMidiNote)
{
    padsArray[pad] = padMidiNote;
}

std::vector<char> Pads::getFxBoardSettings()
{
    return ByteUtil::hexToBytes(fxBoardSettings);
}
