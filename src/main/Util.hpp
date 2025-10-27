#pragma once

#include <string>
#include <vector>
#include <memory>

#include "mpc_types.hpp"
#include "sampler/PgmSlider.hpp"

namespace mpc
{
    class Mpc;
}

namespace mpc::sequencer
{
    class NoteOnEvent;
}

namespace mpc::sampler
{
    class Program;
}

namespace mpc
{
    class Util
    {

    public:
        struct SixteenLevelsContext
        {
            const bool isSixteenLevelsEnabled;
            const int type;
            const int originalKeyPad;
            const int note;
            const int parameter;
            const int padIndexWithoutBank;
        };
        struct SliderNoteVariationContext
        {
            const int sliderValue;
            const int programNote;
            const int sliderParameter;
            const int tuneLowRange;
            const int tuneHighRange;
            const int decayLowRange;
            const int decayHighRange;
            const int attackLowRange;
            const int attackHighRange;
            const int filterLowRange;
            const int filterHighRange;
        };
        static std::string getFileName(const std::string &s);
        static std::vector<std::string> splitName(const std::string &s);
        static std::string distributeTimeSig(const std::string &s);
        static std::string replaceDotWithSmallSpaceDot(const std::string &s);
        static void drawLine(std::vector<std::vector<bool>> &pixels,
                             const LcdLine &line, const bool color);
        static void drawLine(std::vector<std::vector<bool>> &pixels,
                             const LcdLine &line, const bool color,
                             const std::vector<int> &offsetxy);
        static void drawLines(std::vector<std::vector<bool>> &pixels,
                              const LcdBitmap &lines,
                              const std::vector<bool> &colors,
                              const std::vector<int> &offsetxy);
        static std::vector<std::string> &noteNames();
        static std::string tempoString(const double tempo);
        static int getTextWidthInPixels(const std::string &text);
        static void initSequence(mpc::Mpc &mpc);
        static void initSequence(int sequenceIndex, mpc::Mpc &mpc);
        static void
        set16LevelsValues(const SixteenLevelsContext &,
                          const std::shared_ptr<mpc::sequencer::NoteOnEvent>);
        static void setSliderNoteVariationParameters(
            const SliderNoteVariationContext &,
            const std::shared_ptr<mpc::sequencer::NoteOnEvent>);

        static std::vector<char> vecCopyOfRange(const std::vector<char> &src,
                                                int offset, int length);
        static bool vecEquals(const std::vector<char> &a,
                              const std::vector<char> &b);
    };
} // namespace mpc
