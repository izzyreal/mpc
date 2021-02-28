#include <file/all/Tracks.hpp>

#include <Util.hpp>
#include <file/all/AllParser.hpp>
#include <file/all/AllSequence.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>

#include <file/ByteUtil.hpp>
#include <lang/StrUtil.hpp>
#include <VecUtil.hpp>

#include <cmath>

using namespace mpc::file::all;
using namespace std;

Tracks::Tracks(const vector<char>& loadBytes)
{
	for (int i = 0; i < 64; i++) {
		busses[i] = loadBytes[BUSSES_OFFSET + i];
		pgms[i] = loadBytes[PGMS_OFFSET + i];
		veloRatios[i] = loadBytes[VELO_RATIOS_OFFSET + i];
		auto offset = TRACK_NAMES_OFFSET + (i * AllParser::NAME_LENGTH);
		string name = "";
		for (char c : moduru::VecUtil::CopyOfRange(loadBytes, offset, offset + AllParser::NAME_LENGTH)) {
			if (c == 0x00) break;
			name.push_back(c);
		}

		names[i] = name;
		status[i] = loadBytes[STATUS_OFFSET + i];
	}
}

Tracks::Tracks(mpc::sequencer::Sequence* seq)
{
	saveBytes = vector<char>(AllSequence::TRACKS_LENGTH);
	for (int i = 0; i < 64; i++) {
		auto t = seq->getTrack(i).lock();
		for (auto j = 0; j < AllParser::NAME_LENGTH; j++) {
			auto offset = TRACK_NAMES_OFFSET + (i * AllParser::NAME_LENGTH);
			string name = moduru::lang::StrUtil::padRight(t->getActualName(), " ", AllParser::NAME_LENGTH);
			saveBytes[offset + j] = name[j];
		}
		saveBytes[BUSSES_OFFSET + i] = (t->getBus());
		saveBytes[PGMS_OFFSET + i] = (t->getProgramChange());
		saveBytes[VELO_RATIOS_OFFSET + i] = (t->getVelocityRatio());
		auto status = t->isUsed() ? 7 : 6;
		if (t->isUsed() && !t->isOn())
			status = 5;

		saveBytes[STATUS_OFFSET + i] = (status);
	}
	for (int i = 0; i < PADDING1.size(); i++)
		saveBytes[PADDING1_OFFSET + i] = PADDING1[i];

	auto lastTick = static_cast<int>(seq->getLastTick());
	auto remainder = lastTick % 65535;
	auto large = static_cast<int>(floor(lastTick / 65536.0));
	auto lastTickBytes = moduru::file::ByteUtil::ushort2bytes(remainder);
	saveBytes[LAST_TICK_BYTE1_OFFSET] = lastTickBytes[0];
	saveBytes[LAST_TICK_BYTE2_OFFSET] = lastTickBytes[1];
	saveBytes[LAST_TICK_BYTE3_OFFSET] = (large);
	auto unknown32BitIntBytes1 = moduru::file::ByteUtil::uint2bytes(10000000);
	auto unknown32BitIntBytes2 = moduru::file::ByteUtil::uint2bytes((int)(seq->getLastTick() * 5208.333333333333));
	for (int j = 0; j < 4; j++) {
		int offset = UNKNOWN32_BIT_INT_OFFSET + j;
		saveBytes[offset] = unknown32BitIntBytes1[j];
	}
	for (int j = 0; j < 4; j++) {
		saveBytes[UNKNOWN32_BIT_INT_OFFSET + j + 4] = unknown32BitIntBytes2[j];
	}
}

const int Tracks::TRACK_NAMES_OFFSET;
const int Tracks::BUSSES_OFFSET;
const int Tracks::PGMS_OFFSET;
const int Tracks::VELO_RATIOS_OFFSET;
const int Tracks::STATUS_OFFSET;
const int Tracks::PADDING1_OFFSET;
vector<char> Tracks::PADDING1 = vector<char>{ (char) 232, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, (char) 232, 3 };
const int Tracks::LAST_TICK_BYTE1_OFFSET;
const int Tracks::LAST_TICK_BYTE2_OFFSET;
const int Tracks::LAST_TICK_BYTE3_OFFSET;
const int Tracks::UNKNOWN32_BIT_INT_OFFSET;

int Tracks::getBus(int i)
{
    return busses[i];
}

int Tracks::getVelo(int i)
{
    return veloRatios[i];
}

int Tracks::getPgm(int i)
{
    return pgms[i];
}

string Tracks::getName(int i)
{
    return names[i];
}

int Tracks::getStatus(int i)
{
    return status[i];
}

vector<char> Tracks::getBytes()
{
    return saveBytes;
}
