#include "file/kaitai/Mpc60SampleDecoder.hpp"

#include "SampleOps.hpp"

#include <cstdint>
#include <limits>

namespace
{
    // The decoder keeps one signed fixed-point history value. These factors
    // describe the three contributions to the next history and output values;
    // all are represented with 20 fractional bits.
    constexpr int32_t kHistoryFeedback = 647719;
    constexpr int32_t kCurrentStateMix = 374536;
    constexpr int32_t kPreviousStateMix = 25693;
    constexpr int kCoefficientFractionBits = 20;

    uint32_t unsignedBits(const int32_t value)
    {
        if (value >= 0)
        {
            return static_cast<uint32_t>(value);
        }
        return std::numeric_limits<uint32_t>::max() -
               static_cast<uint32_t>(-(value + 1));
    }

    int32_t signedBits(const uint32_t value)
    {
        if (value <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()))
        {
            return static_cast<int32_t>(value);
        }
        return -1 - static_cast<int32_t>(std::numeric_limits<uint32_t>::max() -
                                         value);
    }

    int64_t arithmeticShiftRight(const int64_t value, const int bits)
    {
        const int64_t divisor = int64_t{1} << bits;
        if (value >= 0)
        {
            return value / divisor;
        }
        return -((-value + divisor - 1) / divisor);
    }

    int32_t wrappedAdd(const int32_t left, const int32_t right)
    {
        return signedBits(unsignedBits(left) + unsignedBits(right));
    }

    int32_t fixedMultiply(const int32_t value, const int32_t coefficient)
    {
        const auto scaled =
            arithmeticShiftRight(static_cast<int64_t>(value) * coefficient,
                                 kCoefficientFractionBits);
        return signedBits(static_cast<uint32_t>(scaled));
    }

    int16_t scaleAndClip(const int32_t value)
    {
        const auto coarse = arithmeticShiftRight(value, 8);
        if (coarse >= 32767)
        {
            return 32766;
        }
        if (coarse <= -32767)
        {
            return -32766;
        }

        return static_cast<int16_t>((static_cast<int64_t>(value) * 128) /
                                    32767);
    }
} // namespace

int16_t
mpc::file::kaitai::Mpc60SampleDecoder::decodePcm(const uint16_t canonicalCode)
{
    const auto previousState = state;

    // A sample contributes its signed 12-bit code plus the retained fractional
    // part of the preceding state. Keeping that fraction is what makes decoding
    // continuous across buffers and packed-word boundaries.
    const uint32_t injectedBits =
        (static_cast<uint32_t>(canonicalCode & 0x0fffU) << 20U) |
        (unsignedBits(previousState) & 0x0000ffffU);
    const auto injected = signedBits(injectedBits);
    const auto sampleContribution =
        static_cast<int32_t>(arithmeticShiftRight(injected, 8));

    state = wrappedAdd(sampleContribution,
                       fixedMultiply(previousState, kHistoryFeedback));

    auto mixed = wrappedAdd(fixedMultiply(state, kCurrentStateMix),
                            fixedMultiply(previousState, kPreviousStateMix));
    const auto oneEighth = static_cast<int32_t>(arithmeticShiftRight(mixed, 3));
    mixed = wrappedAdd(mixed, oneEighth);
    mixed = wrappedAdd(
        mixed, static_cast<int32_t>(arithmeticShiftRight(oneEighth, 1)));

    return scaleAndClip(mixed);
}

float mpc::file::kaitai::Mpc60SampleDecoder::decodeFloat(
    const uint16_t canonicalCode)
{
    return mpc::sampleops::short_to_float(decodePcm(canonicalCode));
}

void mpc::file::kaitai::Mpc60SampleDecoder::reset()
{
    state = 0;
}
