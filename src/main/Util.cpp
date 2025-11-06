#include "Util.hpp"

#include "Mpc.hpp"
#include "sequencer/NoteEvent.hpp"

#include "StrUtil.hpp"
#include "lcdgui/screens/UserScreen.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"

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
    for (auto &c : copy)
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

    const size_t i = s.find_last_of('.');
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

void Util::drawLine(std::vector<std::vector<bool>> &pixels, const LcdLine &line,
                    const bool color)
{
    for (auto &l : line)
    {
        pixels[l.first][l.second] = color;
    }
}

void Util::drawLine(std::vector<std::vector<bool>> &pixels, const LcdLine &line,
                    const bool color, const std::vector<int> &offsetxy)
{
    for (auto &l : line)
    {
        pixels[l.first + offsetxy[0]][l.second + offsetxy[1]] = color;
    }
}

void Util::drawLines(std::vector<std::vector<bool>> &pixels,
                     const LcdBitmap &bitmap, const std::vector<bool> &colors,
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

        const std::vector<std::string> someNoteNames{"C.", "C#", "D.", "D#",
                                                     "E.", "F.", "F#", "G.",
                                                     "G#", "A.", "A#", "B."};

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
    const auto sequencer = mpc.getSequencer();
    const auto sequence = sequencer->getSequence(sequenceIndex);

    if (sequence->isUsed())
    {
        return;
    }

    const auto userScreen = mpc.screens->get<ScreenId::UserScreen>();
    sequence->init(userScreen->getLastBar());
    const auto numberString =
        StrUtil::padLeft(std::to_string(sequenceIndex + 1), "0", 2);
    const std::string name =
        StrUtil::trim(sequencer->getDefaultSequenceName()) + numberString;
    sequence->setName(name);
    sequencer->setActiveSequenceIndex(sequencer->getActiveSequenceIndex());
}

void Util::set16LevelsValues(const SixteenLevelsContext &ctx,
                             const std::shared_ptr<NoteOnEvent> &event)
{
    if (!ctx.isSixteenLevelsEnabled)
    {
        return;
    }

    const auto _16l_type = NoteOnEvent::VARIATION_TYPE(ctx.type);
    const auto _16l_key = ctx.originalKeyPad;
    const auto _16l_note = ctx.note;
    const auto _16l_param = ctx.parameter;

    event->setNote(_16l_note);
    event->setVariationType(_16l_type);

    if (_16l_param == 0 && event->getVelocity() != 0)
    {
        const auto velocity =
            static_cast<int>((ctx.padIndexWithoutBank + 1) * (127.0 / 16.0));
        event->setVelocity(velocity);
    }
    else if (_16l_param == 1)
    {
        if (_16l_type != 0)
        {
            const auto value = static_cast<int>(floor(100 / 16.0) *
                                                (ctx.padIndexWithoutBank + 1));
            event->setVariationValue(value);
            return;
        }

        const auto diff = ctx.padIndexWithoutBank - _16l_key;
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

std::pair<NoteOnEvent::VARIATION_TYPE, int>
Util::getSliderNoteVariationTypeAndValue(const SliderNoteVariationContext &ctx)
{
    const auto variationType = NoteOnEvent::VARIATION_TYPE(ctx.sliderParameter);
    const int sliderValue = ctx.sliderValue;

    switch (variationType)
    {
        case 0:
        {
            const auto rangeLow = ctx.tuneLowRange;
            const auto rangeHigh = ctx.tuneHighRange;

            const auto sliderRange = rangeHigh - rangeLow;
            const auto sliderRangeRatio = sliderRange / 128.0;
            auto tuneValue = (int)(sliderValue * sliderRangeRatio * 0.5);
            tuneValue += (120 - rangeHigh) / 2;
            return {variationType, tuneValue};
        }
        case 1:
        {
            const auto rangeLow = ctx.decayLowRange;
            const auto rangeHigh = ctx.decayHighRange;
            const auto sliderRange = rangeHigh - rangeLow;
            const auto sliderRangeRatio = sliderRange / 128.0;
            auto decayValue = (int)(sliderValue * sliderRangeRatio);
            return {variationType, decayValue};
        }
        case 2:
        {
            const auto rangeLow = ctx.attackLowRange;
            const auto rangeHigh = ctx.attackHighRange;
            const auto sliderRange = rangeHigh - rangeLow;
            const auto sliderRangeRatio = sliderRange / 128.0;
            auto attackValue = (int)(sliderValue * sliderRangeRatio);
            return {variationType, attackValue};
        }
        case 3:
        {
            const auto rangeLow = ctx.filterLowRange;
            const auto rangeHigh = ctx.filterHighRange;
            const auto sliderRange = rangeHigh - rangeLow;
            const auto sliderRangeRatio = sliderRange / 128.0;
            auto filterValue = (int)(sliderValue * sliderRangeRatio);
            return {variationType, filterValue};
        }
    }

    return {NoteOnEvent::VARIATION_TYPE::TUNE_0, 0};
}

std::vector<char> Util::vecCopyOfRange(const std::vector<char> &src, int offset,
                                       int endOffset)
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
