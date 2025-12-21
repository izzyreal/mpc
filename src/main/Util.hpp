#pragma once

#include "IntTypes.hpp"

#include <string>
#include <vector>

#include "mpc_types.hpp"
#include "sequencer/EventData.hpp"

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
            const DrumNoteNumber note;
            const int parameter;
            const int padIndexWithoutBank;
        };
        struct SliderNoteVariationContext
        {
            const int sliderValue;
            const NoteVariationType sliderParameter;
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
                             const LcdLine &line, bool color);
        static void drawLine(std::vector<std::vector<bool>> &pixels,
                             const LcdLine &line, bool color,
                             const std::vector<int> &offsetxy);
        static void drawLines(std::vector<std::vector<bool>> &pixels,
                              const LcdBitmap &lines,
                              const std::vector<bool> &colors,
                              const std::vector<int> &offsetxy);
        static std::vector<std::string> &noteNames();
        static std::string tempoString(double tempo);
        static int getTextWidthInPixels(const std::string &text);

        // Delegates to ensureSequenceInitialized with the selected sequence
        // index. See ensureSequenceInitialized for further information.
        static void ensureSelectedSequenceInitialized(mpc::Mpc &mpc);

        // If the selected sequence is currently used, this is a no-op.
        // If it's not used, the selected sequence will be initialized per the
        // USER screen defaults and set to used, after which the sequence is
        // ready for playback and recording.
        static void ensureSequenceInitialized(int sequenceIndex, Mpc &mpc);

        static void set16LevelsValues(const SixteenLevelsContext &,
                                      sequencer::EventData &);

        static std::pair<NoteVariationType, int>
        getSliderNoteVariationTypeAndValue(const SliderNoteVariationContext &);

        static std::vector<char> vecCopyOfRange(const std::vector<char> &src,
                                                int offset, int length);
        static bool vecEquals(const std::vector<char> &a,
                              const std::vector<char> &b);
    };
} // namespace mpc
