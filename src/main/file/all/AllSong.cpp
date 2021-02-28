#include <file/all/AllSong.hpp>

#include <file/all/AllParser.hpp>
#include <sequencer/Song.hpp>

#include <lang/StrUtil.hpp>
#include <VecUtil.hpp>

using namespace mpc::file::all;
using namespace std;

Song::Song(const vector<char>& b) 
{
	auto nameBytes = moduru::VecUtil::CopyOfRange(b, NAME_OFFSET, NAME_OFFSET + AllParser::NAME_LENGTH);
	name = "";
	for (char c : nameBytes) {
		if (c == 0x00) break;
		name.push_back(c);
	}
}

Song::Song(mpc::sequencer::Song* mpcSong)
{
	saveBytes = vector<char>(LENGTH);
	for (int i = 0; i < AllParser::NAME_LENGTH; i++) {
		string name = moduru::lang::StrUtil::padRight(mpcSong->getName(), " ", AllParser::NAME_LENGTH);
		saveBytes[NAME_OFFSET + i] = name[i];
	}

	for (int i = AllParser::NAME_LENGTH; i < LENGTH - 10; i++)
		saveBytes[i] = 0xFF;

	for (int i = LENGTH - 10; i < LENGTH; i++)
		saveBytes[i] = 0;

}

const int Song::LENGTH;
const int Song::NAME_OFFSET;

string Song::getName()
{
    return name;
}

vector<char>& Song::getBytes()
{
    return saveBytes;
}
