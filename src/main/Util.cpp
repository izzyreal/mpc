#include <Util.hpp>

#include "sequencer/NoteEvent.hpp"

#include <cmath>
#include <cstdlib>

#include <StrUtil.hpp>
#include "lcdgui/screens/UserScreen.hpp"

using namespace mpc;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::sequencer;
using namespace mpc::sampler;

std::string Util::replaceDotWithSmallSpaceDot(const std::string &s)
{
    const auto dotindex = static_cast<int>(s.find('.'));
    const auto part1 = s.substr(0, dotindex);
    const auto part2 = s.substr(dotindex + 1);
    const std::string tempoDot = u8"\u00CB";
    return part1 + tempoDot + part2;
}

std::string Util::getFileName(const std::string &s)
{
    std::string copy = s;
    copy = StrUtil::trim(copy);
    for (auto c : copy)
    {
        c = toupper(c);
        if (c == ' ')
        {
            c = '_';
        }
    }
    return copy;
}

std::vector<std::string> Util::splitName(const std::string &s)
{
    if (s.find('.') == std::string::npos)
    {
        std::vector<std::string> res(2);
        res[0] = s;
        res[1] = "";
        return res;
    }

    size_t i = s.find_last_of('.');
    std::vector<std::string> res(2);
    res[0] = s.substr(0, i);
    res[1] = s.substr(i + 1);
    return res;
}

std::string Util::distributeTimeSig(const std::string &s)
{
    const auto pos = s.find("/");

    if (pos == std::string::npos)
    {
        return s;
    }

    auto s0 = s.substr(0, pos);
    auto s1 = s.substr(pos + 1, s.length());

    if (s0.length() == 1)
    {
        s0 = u8"\u00CE" + s0 + u8"\u00CE";
    }

    if (s1.length() == 1)
    {
        s1 = u8"\u00CE" + s1;
    }

    return s0 + "/" + s1;
}

void Util::drawLine(std::vector<std::vector<bool>> &pixels,
                    const LcdLine &line,
                    const bool color)
{
    for (auto &l : line)
    {
        pixels[l.first][l.second] = color;
    }
}

void Util::drawLine(std::vector<std::vector<bool>> &pixels,
                    const LcdLine &line,
                    const bool color,
                    const std::vector<int> &offsetxy)
{
    for (auto &l : line)
    {
        pixels[l.first + offsetxy[0]][l.second + offsetxy[1]] = color;
    }
}

void Util::drawLines(std::vector<std::vector<bool>> &pixels,
                     const LcdBitmap &bitmap,
                     const std::vector<bool> &colors,
                     const std::vector<int> &offsetxy)
{
    int colorCounter = 0;
    for (auto &line : bitmap)
    {
        drawLine(pixels, line, colors[colorCounter++], offsetxy);
    }
}

std::vector<std::string> &Util::noteNames()
{
    static std::vector<std::string> noteNames;

    if (noteNames.empty())
    {
        noteNames = std::vector<std::string>(128);

        int octave = -2;
        int noteCounter = 0;

        std::vector<std::string> someNoteNames{"C.", "C#", "D.", "D#", "E.", "F.", "F#", "G.", "G#", "A.", "A#", "B."};

        for (int j = 0; j < 128; j++)
        {
            std::string octaveString = std::to_string(octave);

            if (octave == -2)
            {
                octaveString = u8"\u00D2";
            }

            if (octave == -1)
            {
                octaveString = u8"\u00D3";
            }

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

std::string Util::tempoString(const double tempo)
{
    std::string result = std::to_string(tempo);

    if (result.find(".") == std::string::npos)
    {
        result += ".0";
    }
    else
    {
        result = result.substr(0, result.find(".") + 2);
    }

    return replaceDotWithSmallSpaceDot(result);
}

int Util::getTextWidthInPixels(const std::string &text)
{
    const char *p = text.c_str();

    int count = 0;

    for (; *p != 0; ++p)
    {
        count += (*p & 0xc0) != 0x80;
    }

    int halfSpaceCount = 0;

    const std::string halfSpace = u8"\u00CE";

    int nPos = text.find(halfSpace, 0);

    while (nPos != std::string::npos)
    {
        halfSpaceCount++;
        nPos = text.find(halfSpace, nPos + halfSpace.size());
    }

    return (count * 6) - (halfSpaceCount * 3);
}

void Util::initSequence(mpc::Mpc &mpc)
{
    initSequence(mpc.getSequencer()->getActiveSequenceIndex(), mpc);
}

void Util::initSequence(int sequenceIndex, mpc::Mpc &mpc)
{
    auto sequencer = mpc.getSequencer();
    auto sequence = sequencer->getSequence(sequenceIndex);

    if (sequence->isUsed())
    {
        return;
    }

    auto userScreen = mpc.screens->get<UserScreen>();
    sequence->init(userScreen->lastBar);
    auto numberString = StrUtil::padLeft(std::to_string(sequenceIndex + 1), "0", 2);
    std::string name = StrUtil::trim(sequencer->getDefaultSequenceName()) + numberString;
    sequence->setName(name);
    sequencer->setActiveSequenceIndex(sequencer->getActiveSequenceIndex());
}

void Util::set16LevelsValues(const SixteenLevelsContext &ctx, const std::shared_ptr<NoteOnEvent> event)
{
    if (!ctx.isSixteenLevelsEnabled)
    {
        return;
    }

    auto _16l_type = NoteOnEvent::VARIATION_TYPE(ctx.type);
    auto _16l_key = ctx.originalKeyPad;
    auto _16l_note = ctx.note;
    auto _16l_param = ctx.parameter;

    event->setNote(_16l_note);
    event->setVariationType(_16l_type);

    if (_16l_param == 0 && event->getVelocity() != 0)
    {
        auto velocity = static_cast<int>((ctx.padIndexWithoutBank + 1) * (127.0 / 16.0));
        event->setVelocity(velocity);
    }
    else if (_16l_param == 1)
    {
        if (_16l_type != 0)
        {
            auto value = static_cast<int>(floor(100 / 16.0) * (ctx.padIndexWithoutBank + 1));
            event->setVariationValue(value);
            return;
        }

        auto diff = ctx.padIndexWithoutBank - _16l_key;
        auto candidate = 64 + (diff * 5);

        if (candidate > 124)
        {
            candidate = 124;
        }
        else if (candidate < 4)
        {
            candidate = 4;
        }

        event->setVariationValue(candidate);
    }
}

void Util::setSliderNoteVariationParameters(const SliderNoteVariationContext &ctx, const std::shared_ptr<NoteOnEvent> noteOnEvent)
{
    const auto sliderParam = NoteOnEvent::VARIATION_TYPE(ctx.sliderParameter);
    noteOnEvent->setVariationType(sliderParam);
    int sliderValue = ctx.sliderValue;

    switch (sliderParam)
    {
    case 0:
    {
        auto rangeLow = ctx.tuneLowRange;
        auto rangeHigh = ctx.tuneHighRange;

        auto sliderRange = rangeHigh - rangeLow;
        auto sliderRangeRatio = sliderRange / 128.0;
        auto tuneValue = (int)(sliderValue * sliderRangeRatio * 0.5);
        tuneValue += (120 - rangeHigh) / 2;
        noteOnEvent->setVariationValue(tuneValue);
        break;
    }
    case 1:
    {
        auto rangeLow = ctx.decayLowRange;
        auto rangeHigh = ctx.decayHighRange;
        auto sliderRange = rangeHigh - rangeLow;
        auto sliderRangeRatio = sliderRange / 128.0;
        auto decayValue = (int)(sliderValue * sliderRangeRatio);
        noteOnEvent->setVariationValue(decayValue);
        break;
    }
    case 2:
    {
        auto rangeLow = ctx.attackLowRange;
        auto rangeHigh = ctx.attackHighRange;
        auto sliderRange = rangeHigh - rangeLow;
        auto sliderRangeRatio = sliderRange / 128.0;
        auto attackValue = (int)(sliderValue * sliderRangeRatio);
        noteOnEvent->setVariationValue(attackValue);
        break;
    }
    case 3:
    {
        auto rangeLow = ctx.filterLowRange;
        auto rangeHigh = ctx.filterHighRange;
        auto sliderRange = rangeHigh - rangeLow;
        auto sliderRangeRatio = sliderRange / 128.0;
        auto filterValue = (int)(sliderValue * sliderRangeRatio);
        noteOnEvent->setVariationValue(filterValue);
        break;
    }
    }
}

std::vector<char> Util::vecCopyOfRange(const std::vector<char> &src, int offset, int endOffset)
{
    return {begin(src) + offset, begin(src) + endOffset};
}

bool Util::vecEquals(const std::vector<char> &a, const std::vector<char> &b)
{
    if (a.size() != b.size())
    {
        return false;
    }
    for (int i = 0; i < a.size(); i++)
    {
        if (a[i] != b[i])
        {
            return false;
        }
    }
    return true;
}
