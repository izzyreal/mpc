#include <file/all/MidiSyncMisc.hpp>

#include <Mpc.hpp>
#include <file/all/AllParser.hpp>

#include <lang/StrUtil.hpp>
#include <VecUtil.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/SongScreen.hpp>
#include <lcdgui/screens/SyncScreen.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;
using namespace mpc::file::all;
using namespace std;

MidiSyncMisc::MidiSyncMisc(vector<char> b) 
{
	inMode = b[IN_MODE_OFFSET];
	outMode = b[OUT_MODE_OFFSET];
	shiftEarly = b[SHIFT_EARLY_OFFSET];
	sendMMCEnabled = b[SEND_MMC_OFFSET] > 0;
	frameRate = b[FRAME_RATE_OFFSET];
	input = b[INPUT_OFFSET];
	output = b[OUTPUT_OFFSET];

	auto stringBuffer = moduru::VecUtil::CopyOfRange(&b, DEF_SONG_NAME_OFFSET, DEF_SONG_NAME_OFFSET + AllParser::NAME_LENGTH);
	defSongName = string(stringBuffer.begin(), stringBuffer.end());
}

MidiSyncMisc::MidiSyncMisc(mpc::Mpc& mpc)
{
	saveBytes = vector<char>(LENGTH);

	auto syncScreen = dynamic_pointer_cast<SyncScreen>(mpc.screens->getScreenComponent("sync"));

	saveBytes[IN_MODE_OFFSET] = static_cast<int8_t>(syncScreen->getModeIn());
	saveBytes[OUT_MODE_OFFSET] = static_cast<int8_t>(syncScreen->getModeOut());
	saveBytes[SHIFT_EARLY_OFFSET] = static_cast<int8_t>(syncScreen->shiftEarly);
	saveBytes[SEND_MMC_OFFSET] = static_cast<int8_t>((syncScreen->sendMMCEnabled ? 1 : 0));
	saveBytes[FRAME_RATE_OFFSET] = static_cast<int8_t>(syncScreen->frameRate);
	saveBytes[INPUT_OFFSET] = static_cast<int8_t>(syncScreen->in);
	saveBytes[OUTPUT_OFFSET] = static_cast<int8_t>(syncScreen->out);

	auto songScreen = dynamic_pointer_cast<SongScreen>(mpc.screens->getScreenComponent("song"));

	for (int i = 0; i < AllParser::NAME_LENGTH; i++)
	{
		saveBytes[DEF_SONG_NAME_OFFSET + i] = moduru::lang::StrUtil::padRight(songScreen->getDefaultSongName(), " ", 16)[i];
	}

	saveBytes[DEF_SONG_NAME_OFFSET + 16] = 1;
}

const int MidiSyncMisc::LENGTH;
const int MidiSyncMisc::IN_MODE_OFFSET;
const int MidiSyncMisc::OUT_MODE_OFFSET;
const int MidiSyncMisc::SHIFT_EARLY_OFFSET;
const int MidiSyncMisc::SEND_MMC_OFFSET;
const int MidiSyncMisc::FRAME_RATE_OFFSET;
const int MidiSyncMisc::INPUT_OFFSET;
const int MidiSyncMisc::OUTPUT_OFFSET;
const int MidiSyncMisc::DEF_SONG_NAME_OFFSET;

int MidiSyncMisc::getInMode()
{
    return inMode;
}

int MidiSyncMisc::getOutMode()
{
    return outMode;
}

int MidiSyncMisc::getShiftEarly()
{
    return shiftEarly;
}

bool MidiSyncMisc::isSendMMCEnabled()
{
    return sendMMCEnabled;
}

int MidiSyncMisc::getFrameRate()
{
    return frameRate;
}

int MidiSyncMisc::getInput()
{
    return input;
}

int MidiSyncMisc::getOutput()
{
    return output;
}

string MidiSyncMisc::getDefSongName()
{
    return defSongName;
}

vector<char> MidiSyncMisc::getBytes()
{
    return saveBytes;
}
