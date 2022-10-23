#include <Util.hpp>

#include <sequencer/NoteEvent.hpp>
#include <sampler/Program.hpp>
#include <lcdgui/screens/window/Assign16LevelsScreen.hpp>
#include <hardware/Hardware.hpp>
#include <hardware/HwSlider.hpp>
#include <hardware/TopPanel.hpp>

#include <cmath>
#include <cstdlib>

#include <lang/StrUtil.hpp>
#include <lcdgui/screens/UserScreen.hpp>

using namespace std;
using namespace mpc;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;
using namespace mpc::sampler;
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
    
    if (xPos == -1 || yPos == -1)
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
    if (s.find('.') == string::npos)
    {
        vector<string> res(2);
        res[0] = s;
        res[1] = "";
        return res;
    }
    
    size_t i = s.find_last_of('.');
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
    
    if (noteNames.empty())
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
    
    for (; *p != 0; ++p)
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
    initSequence(mpc.getSequencer().lock()->getActiveSequenceIndex(), mpc);
}

void Util::initSequence(int sequenceIndex, mpc::Mpc& mpc)
{
    auto sequencer = mpc.getSequencer().lock();
    auto sequence = sequencer->getSequence(sequenceIndex);
    
    if (sequence->isUsed())
        return;
    
    auto userScreen = mpc.screens->get<UserScreen>("user");
    sequence->init(userScreen->lastBar);
    auto numberString = StrUtil::padLeft(to_string(sequenceIndex + 1), "0", 2);
    std::string name = StrUtil::trim(sequencer->getDefaultSequenceName()) + numberString;
    sequence->setName(name);
    sequencer->setActiveSequenceIndex(sequencer->getActiveSequenceIndex());
}

void Util::set16LevelsValues(mpc::Mpc& mpc, const shared_ptr<NoteEvent>& event, const int padIndex)
{
    if (mpc.getHardware().lock()->getTopPanel().lock()->isSixteenLevelsEnabled())
    {
        auto assign16LevelsScreen = mpc.screens->get<Assign16LevelsScreen>("assign-16-levels");
        
        auto _16l_type = assign16LevelsScreen->getType();
        auto _16l_key = assign16LevelsScreen->getOriginalKeyPad();
        auto _16l_note = assign16LevelsScreen->getNote();
        auto _16l_param = assign16LevelsScreen->getParameter();
        
        event->setNote(_16l_note);
        event->setVariationTypeNumber(_16l_type);
        
        if (_16l_param == 0 && event->getVelocity() != 0)
        {
            auto velocity = static_cast<int>((padIndex + 1) * (127.0 / 16.0));
            event->setVelocity(velocity);
        }
        else if (_16l_param == 1)
        {
            if (_16l_type != 0)
            {
                auto value = static_cast<int>(floor(100 / 16.0) * (padIndex + 1));
                event->setVariationValue(value);
                return;
            }
            
            auto diff = padIndex - _16l_key;
            auto candidate = 64 + (diff * 5);
            
            if (candidate > 124)
                candidate = 124;
            else if (candidate < 4)
                candidate = 4;
            
            event->setVariationValue(candidate);
        }
    }
}

void Util::setSliderNoteVariationParameters(mpc::Mpc& mpc, const weak_ptr<NoteEvent>& _n, const weak_ptr<mpc::sampler::Program>& program)
{
    auto pgm = program.lock();
    auto n = _n.lock();
    
    if (n->getNote() != pgm->getSlider()->getNote())
        return;
    
    auto sliderParam = pgm->getSlider()->getParameter();
    n->setVariationTypeNumber(sliderParam);
    int sliderValue = mpc.getHardware().lock()->getSlider().lock()->getValue();
    
    switch (sliderParam)
    {
        case 0:
        {
            auto rangeLow = pgm->getSlider()->getTuneLowRange();
            auto rangeHigh = pgm->getSlider()->getTuneHighRange();
            
            auto sliderRange = rangeHigh - rangeLow;
            auto sliderRangeRatio = sliderRange / 128.0;
            auto tuneValue = (int)(sliderValue * sliderRangeRatio * 0.5);
            tuneValue += (120 - rangeHigh) / 2;
            n->setVariationValue(tuneValue);
            break;
        }
        case 1:
        {
            auto rangeLow = pgm->getSlider()->getDecayLowRange();
            auto rangeHigh = pgm->getSlider()->getDecayHighRange();
            auto sliderRange = rangeHigh - rangeLow;
            auto sliderRangeRatio = sliderRange / 128.0;
            auto decayValue = (int)(sliderValue * sliderRangeRatio);
            n->setVariationValue(decayValue);
            break;
        }
        case 2:
        {
            auto rangeLow = pgm->getSlider()->getAttackLowRange();
            auto rangeHigh = pgm->getSlider()->getAttackHighRange();
            auto sliderRange = rangeHigh - rangeLow;
            auto sliderRangeRatio = sliderRange / 128.0;
            auto attackValue = (int)(sliderValue * sliderRangeRatio);
            n->setVariationValue(attackValue);
            break;
        }
        case 3:
        {
            auto rangeLow = pgm->getSlider()->getFilterLowRange();
            auto rangeHigh = pgm->getSlider()->getFilterHighRange();
            auto sliderRange = rangeHigh - rangeLow;
            auto sliderRangeRatio = sliderRange / 128.0;
            auto filterValue = (int)(sliderValue * sliderRangeRatio);
            n->setVariationValue(filterValue);
            break;
        }
    }
}
