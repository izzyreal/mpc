#include <Util.hpp>

#include <cmath>
#include <cstdlib>

#include <lang/StrUtil.hpp>
#include <lcdgui/screens/UserScreen.hpp>

using namespace std;
using namespace mpc;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace moduru::lang;

string Util::replaceDotWithSmallSpaceDot(const string& s) {
	const auto dotindex = static_cast<int>(s.find('.'));
	const auto part1 = s.substr(0, dotindex);
	const auto part2 = s.substr(dotindex + 1);
    const string tempoDot = u8"\u00CB";
    return part1 + tempoDot + part2;
}

vector<int> Util::getPadAndVelo(const int x, const int y)
{
	int velocity;
	int padSize = 93;
	int emptySize = 23;
	int padPosX = 785;
	int padPosY = 343;
	int xPos = -1;
	int yPos = -1;

	for (int j = 0; j < 4; j++)
	{
		for (int i = 0; i < 4; i++)
		{
			int xborderl = padPosX + (i * padSize) + (i * (emptySize + (i * 2)));
			int xborderr = xborderl + padSize;
			int yborderu = padPosY + (j * padSize) + (j * (emptySize + (j * 2)));
			int yborderd = yborderu + padSize;
			int centerx = xborderl + (padSize / 2);
			int centery = yborderu + (padSize / 2);
		
			if (x > xborderl && x < xborderr && y > yborderu && y < yborderd)
			{
				xPos = i;
				yPos = j;
				int distcx = abs(centerx - x);
				int distcy = abs(centery - y);
				velocity = 127 - ((127.0 / 46.0) * ((distcx + distcy) / 2.0));
				break;
			}
		}
	}

    if (yPos == -1 || yPos == -1)
        return vector<int>{ -1, -1 };
 
	int padNumber = -1;
	vector<int> column0 = { 12, 8, 4, 0 };
	vector<int> column1 = { 13, 9, 5, 1 };
	vector<int> column2 = { 14, 10, 6, 2 };
	vector<int> column3 = { 15, 11, 7, 3 };
	auto columns = vector<vector<int>>(4);
	columns[0] = column0;
	columns[1] = column1;
	columns[2] = column2;
	columns[3] = column3;
	padNumber = columns[xPos][yPos];
	return vector<int>{ (int)padNumber, (int)velocity };
}

string Util::getFileName(const string& s)
{
	string copy = s;
	copy = StrUtil::trim(copy);
	for (auto c : copy)
	{
		c = toupper(c);
        if (c == ' ')
            c = '_';
	}
	return copy;
}

vector<string> Util::splitName(const string& s)
{
	if (s.find(".") == string::npos)
	{
		vector<string> res(2);
		res[0] = s;
		res[1] = "";
		return res;
	}

	size_t i = s.find_last_of(".");
	vector<string> res(2);
	res[0] = s.substr(0, i);
	res[1] = s.substr(i + 1);
	return res;
}

string Util::distributeTimeSig(const string& s)
{
	const auto pos = s.find("/");
 
	if (pos == string::npos)
        return s;
 
	auto s0 = s.substr(0, pos);
	auto s1 = s.substr(pos + 1, s.length());

	if (s0.length() == 1)
        s0 = u8"\u00CE" + s0 + u8"\u00CE";
    
	if (s1.length() == 1)
        s1 = u8"\u00CE" + s1;

	return s0 + "/" + s1;
}

void Util::drawLine(vector<vector<bool>>& pixels,
                    const vector<vector<int>>& line,
                    const bool color)
{
	for (auto& l : line)
		pixels[l[0]][l[1]] = color;
}

void Util::drawLine(vector<vector<bool>>& pixels,
                    const vector<vector<int>>& line,
                    const bool color,
                    const vector<int>& offsetxy)
{
	for (auto& l : line)
		pixels[l[0] + offsetxy[0]][l[1] + offsetxy[1]] = color;
}

void Util::drawLines(vector<vector<bool>>& pixels,
                     const vector<vector<vector<int>>>& lines,
                     const vector<bool>& colors)
{
	int colorCounter = 0;
	
	for (auto& l : lines)
		drawLine(pixels, l, colors[colorCounter++]);
}

void Util::drawLines(vector<vector<bool>>& pixels,
                     const vector<vector<vector<int>>>& lines,
                     const vector<bool>& colors,
                     const vector<int>& offsetxy)
{
	int colorCounter = 0;
	for (auto& l : lines)
		drawLine(pixels, l, colors[colorCounter++], offsetxy);
}

vector<string>& Util::noteNames()
{
	static vector<string> noteNames;

	if (noteNames.size() == 0)
	{
		noteNames = vector<string>(128);
		
		int octave = -2;
		int noteCounter = 0;

		std::vector<std::string> someNoteNames{ "C.", "C#", "D.", "D#", "E.", "F.", "F#", "G.", "G#", "A.", "A#", "B." };

		for (int j = 0; j < 128; j++)
		{
			string octaveString = to_string(octave);

			if (octave == -2)
				octaveString = u8"\u00D2";

			if (octave == -1)
				octaveString = u8"\u00D3";

			noteNames[j] = someNoteNames[noteCounter] + octaveString;
			noteCounter++;

			if (noteCounter == 12)
			{
				noteCounter = 0;
				octave++;
			}
		}
	}
	
	return noteNames;
}

string Util::tempoString(const double tempo)
{
	string result = to_string(tempo);
	
	if (result.find(".") == string::npos)
		result += ".0";
	else
		result = result.substr(0, result.find(".") + 2);

	return replaceDotWithSmallSpaceDot(result);
}

int Util::getTextWidthInPixels(const string& text)
{
	const char* p = text.c_str();

	int count = 0;

	for (p; *p != 0; ++p)
		count += (*p & 0xc0) != 0x80;

	int halfSpaceCount = 0;

	const string halfSpace = u8"\u00CE";

	int nPos = text.find(halfSpace, 0);

	while (nPos != string::npos)
	{
		halfSpaceCount++;
		nPos = text.find(halfSpace, nPos + halfSpace.size());
	}

	return (count * 6) - (halfSpaceCount * 3);
}

void Util::initSequence(mpc::Mpc& mpc)
{
	auto sequencer = mpc.getSequencer().lock();
	auto sequence = sequencer->getActiveSequence().lock();

	if (sequence->isUsed())
		return;

	auto userScreen = mpc.screens->get<UserScreen>("user");
	sequence->init(userScreen->lastBar);
	int index = sequencer->getActiveSequenceIndex();
	string name = StrUtil::trim(sequencer->getDefaultSequenceName()) + StrUtil::padLeft(to_string(index + 1), "0", 2);
	sequence->setName(name);
	sequencer->setActiveSequenceIndex(sequencer->getActiveSequenceIndex());
}
