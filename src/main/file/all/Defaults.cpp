#include <file/all/Defaults.hpp>

#include <Util.hpp>
#include <file/all/AllParser.hpp>
#include <sequencer/TimeSignature.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/UserScreen.hpp>

#include <file/ByteUtil.hpp>
#include <lang/StrUtil.hpp>
#include <VecUtil.hpp>

using namespace mpc::file::all;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace moduru::file;
using namespace moduru::lang;
using namespace std;

Defaults::Defaults(mpc::Mpc& mpc, const vector<char>& loadBytes)
	: mpc(mpc)
{
	parseNames(loadBytes);

	auto tempoBytes = vector<char>{ loadBytes[TEMPO_BYTE1_OFFSET], loadBytes[TEMPO_BYTE2_OFFSET] };
	tempo = ByteUtil::bytes2ushort(tempoBytes);
	timeSigNum = loadBytes[TIMESIG_NUM_OFFSET];
	timeSigDen = loadBytes[TIMESIG_DEN_OFFSET];
    
	auto barCountBytes = vector<char>{ loadBytes[BAR_COUNT_BYTE1_OFFSET], loadBytes[BAR_COUNT_BYTE2_OFFSET] };

    barCount = ByteUtil::bytes2ushort(barCountBytes);

	for (int i = 0; i < 64; i++)
	{
		devices[i] = loadBytes[DEVICES_OFFSET + i];
		busses[i] = loadBytes[BUSSES_OFFSET + i];
		pgms[i] = loadBytes[PGMS_OFFSET + i];
		trVelos[i] = loadBytes[TR_VELOS_OFFSET + i];
		status[i] = loadBytes[TR_STATUS_OFFSET + i];
	}
}

Defaults::Defaults(mpc::Mpc& mpc)
	: mpc(mpc)
{
	saveBytes = vector<char>(AllParser::DEFAULTS_LENGTH);
	
	setNames();
	
	for (int i = 0; i < UNKNOWN1.size(); i++)
		saveBytes[UNKNOWN1_OFFSET + i] = UNKNOWN1[i];

	setTempo();
	setTimeSig();
	setBarCount();
	setLastTick();

	auto userScreen = mpc.screens->get<UserScreen>("user");
	auto lastBar = userScreen->lastBar;
	
	if (lastBar == 1)
	{
		saveBytes[LAST_TICK_BYTE1_OFFSET] = 0;
		saveBytes[LAST_TICK_BYTE2_OFFSET] = 0;
		lastBar = 0;
	}

	auto unknownNumberBytes = moduru::file::ByteUtil::uint2bytes((lastBar + 1) * 2000000);

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			saveBytes[UNKNOWN32_BIT_INT_OFFSET + j + (i * 4)] = unknownNumberBytes[j];
	}

	for (int i = 0; i < UNKNOWN2.size(); i++)
		saveBytes[UNKNOWN2_OFFSET + i] = UNKNOWN2[i];

	setTrackSettings();
}

const int Defaults::DEF_SEQ_NAME_OFFSET;
const int Defaults::UNKNOWN1_OFFSET;

vector<char> Defaults::UNKNOWN1 = vector<char>{ 1, 0, 0, 1, 1, 0 };

const int Defaults::TEMPO_BYTE1_OFFSET;
const int Defaults::TEMPO_BYTE2_OFFSET;
const int Defaults::TIMESIG_NUM_OFFSET;
const int Defaults::TIMESIG_DEN_OFFSET;
const int Defaults::BAR_COUNT_BYTE1_OFFSET;
const int Defaults::BAR_COUNT_BYTE2_OFFSET;
const int Defaults::LAST_TICK_BYTE1_OFFSET;
const int Defaults::LAST_TICK_BYTE2_OFFSET;
const int Defaults::LAST_TICK_BYTE3_OFFSET;
const int Defaults::UNKNOWN32_BIT_INT_OFFSET;
const int Defaults::UNKNOWN2_OFFSET;

vector<char> Defaults::UNKNOWN2 = vector<char>{ 0, 0, (char) 0xFF, (char) 0xFF, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 32, 32, 32, 32, 32, 32, 32 };

const int Defaults::DEV_NAMES_OFFSET;
const int Defaults::DEV_NAMES_LENGTH;
const int Defaults::TR_NAMES_OFFSET;
const int Defaults::TR_NAMES_LENGTH;
const int Defaults::DEVICES_OFFSET;
const int Defaults::DEVICES_LENGTH;
const int Defaults::BUSSES_OFFSET;
const int Defaults::BUSSES_LENGTH;
const int Defaults::PGMS_OFFSET;
const int Defaults::PGMS_LENGTH;
const int Defaults::TR_VELOS_OFFSET;
const int Defaults::TR_VELOS_LENGTH;
const int Defaults::TR_STATUS_OFFSET;
const int Defaults::TR_STATUS_LENGTH;

void Defaults::parseNames(vector<char> loadBytes)
{
	vector<char> stringBuffer;
	stringBuffer = moduru::VecUtil::CopyOfRange(loadBytes, DEF_SEQ_NAME_OFFSET, DEF_SEQ_NAME_OFFSET + AllParser::NAME_LENGTH);
	defaultSeqName = "";
	for (char c : stringBuffer) {
		if (c == 0x00) break;
		defaultSeqName.push_back(c);
	}
	auto offset = 0;
	for (int i = 0; i < 33; i++) {
		offset = DEV_NAMES_OFFSET + (i * AllParser::DEV_NAME_LENGTH);
		stringBuffer = moduru::VecUtil::CopyOfRange(loadBytes, offset, offset + AllParser::DEV_NAME_LENGTH);
		string s = "";
		for (char c : stringBuffer) {
			if (c == 0x00) break;
			s.push_back(c);
		}
		devNames[i] = s;
	}
	for (int i = 0; i < 64; i++) {
		offset = TR_NAMES_OFFSET + (i * AllParser::NAME_LENGTH);
		stringBuffer = moduru::VecUtil::CopyOfRange(loadBytes, offset, offset + AllParser::NAME_LENGTH);
		string s = "";
		for (char c : stringBuffer) {
			if (c == 0x00) break;
			s.push_back(c);
		}
		trackNames[i] = s;
	}
}

string Defaults::getDefaultSeqName()
{
    return defaultSeqName;
}

int Defaults::getTempo()
{
    return tempo;
}

int Defaults::getTimeSigNum()
{
    return timeSigNum;
}

int Defaults::getTimeSigDen()
{
    return timeSigDen;
}

int Defaults::getBarCount()
{
    return barCount;
}

vector<string> Defaults::getDefaultDevNames()
{
    return devNames;
}

vector<string> Defaults::getDefaultTrackNames()
{
    return trackNames;
}

vector<int> Defaults::getDevices()
{
    return devices;
}

vector<int> Defaults::getBusses()
{
    return busses;
}

vector<int> Defaults::getPgms()
{
    return pgms;
}

vector<int> Defaults::getTrVelos()
{
    return trVelos;
}

void Defaults::setTrackSettings()
{
	auto userScreen = mpc.screens->get<UserScreen>("user");
    for (int i = 0; i < 64; i++)
	{
        saveBytes[DEVICES_OFFSET + i] = (userScreen->device);
        saveBytes[BUSSES_OFFSET + i] = (userScreen->bus);
        saveBytes[PGMS_OFFSET + i] = (userScreen->pgm);
        saveBytes[TR_VELOS_OFFSET + i] = (userScreen->velo);
        saveBytes[TR_STATUS_OFFSET + i] = (userScreen->getTrackStatus());
    }
}

void Defaults::setLastTick()
{
	auto userScreen = mpc.screens->get<UserScreen>("user");
	auto lastTick = (userScreen->lastBar + 1) * 384;

	auto b = moduru::file::ByteUtil::ushort2bytes(lastTick);
	
	saveBytes[LAST_TICK_BYTE1_OFFSET] = b[0];
	saveBytes[LAST_TICK_BYTE2_OFFSET] = b[1];
}

void Defaults::setBarCount()
{
	auto userScreen = mpc.screens->get<UserScreen>("user");
	auto ba = moduru::file::ByteUtil::ushort2bytes(userScreen->lastBar + 1);
	saveBytes[BAR_COUNT_BYTE1_OFFSET] = ba[0];
	saveBytes[BAR_COUNT_BYTE2_OFFSET] = ba[1];
}

void Defaults::setTimeSig()
{
	auto userScreen = mpc.screens->get<UserScreen>("user");
	saveBytes[TIMESIG_NUM_OFFSET] = (userScreen->timeSig.getNumerator());
	saveBytes[TIMESIG_DEN_OFFSET] = (userScreen->timeSig.getDenominator());
}

void Defaults::setNames()
{
	auto userScreen = mpc.screens->get<UserScreen>("user");
	auto const defSeqName =StrUtil::padRight(userScreen->sequenceName, " ", AllParser::NAME_LENGTH);
	
	for (int i = 0; i < 16; i++)
	{
		saveBytes[DEF_SEQ_NAME_OFFSET + i] = defSeqName[i];
	}

	string stringBuffer;

	for (int i = 0; i < 33; i++)
	{
		auto const defDevName = userScreen->getDeviceName(i);
		stringBuffer =StrUtil::padRight(defDevName, " ", AllParser::DEV_NAME_LENGTH);
		auto offset = DEV_NAMES_OFFSET + (i * AllParser::DEV_NAME_LENGTH);
	
		for (int j = offset; j < offset + AllParser::DEV_NAME_LENGTH; j++)
		{
			saveBytes[j] = stringBuffer[j - offset];
		}

	}
	for (int i = 0; i < 64; i++)
	{
		auto const defTrackName = userScreen->getTrackName(i);
		stringBuffer = StrUtil::padRight(defTrackName, " ", AllParser::NAME_LENGTH);
		auto offset = TR_NAMES_OFFSET + (i * AllParser::NAME_LENGTH);
	
		for (int j = offset; j < offset + AllParser::NAME_LENGTH; j++)
		{
			saveBytes[j] = stringBuffer[j - offset];
		}
	}
}

void Defaults::setTempo()
{
	auto userScreen = mpc.screens->get<UserScreen>("user");
	auto tempo = static_cast<int>(userScreen->tempo * 10.0);
	auto tempoBytes = moduru::file::ByteUtil::ushort2bytes(tempo);
	saveBytes[TEMPO_BYTE1_OFFSET] = tempoBytes[0];
	saveBytes[TEMPO_BYTE2_OFFSET] = tempoBytes[1];
}

vector<char> Defaults::getBytes()
{
	return saveBytes;
}
