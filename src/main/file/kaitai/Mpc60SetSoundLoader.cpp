#include "file/kaitai/Mpc60SetSoundLoader.hpp"

#include "SampleOps.hpp"
#include "disk/MpcFile.hpp"
#include "file/kaitai/generated/mpc60_set_v1.h"
#include "sampler/Sound.hpp"

#include <kaitai/kaitaistream.h>

#include <cstdint>
#include <initializer_list>
#include <sstream>
#include <utility>

namespace
{
    constexpr int kMpc60SampleRate = 40000;
    constexpr uint16_t kMpc2000xlSetImportScale = 0x7fff;

    std::string trimRightSpaces(std::string value)
    {
        while (!value.empty() && value.back() == ' ')
        {
            value.pop_back();
        }
        return value;
    }

    uint16_t u16(const uint32_t value)
    {
        return static_cast<uint16_t>(value & 0xffffU);
    }

    int16_t s16(const uint32_t value)
    {
        return static_cast<int16_t>(u16(value));
    }

    uint16_t cwd(const uint16_t ax)
    {
        return (ax & 0x8000U) != 0 ? 0xffffU : 0U;
    }

    uint16_t shl1(uint16_t &value)
    {
        const auto carry = static_cast<uint16_t>((value & 0x8000U) != 0);
        value = u16(static_cast<uint32_t>(value) << 1U);
        return carry;
    }

    uint16_t rcl1(uint16_t &value, const uint16_t carryIn)
    {
        const auto carry = static_cast<uint16_t>((value & 0x8000U) != 0);
        value = u16((static_cast<uint32_t>(value) << 1U) | (carryIn & 1U));
        return carry;
    }

    uint16_t sar1(uint16_t &value)
    {
        const auto carry = static_cast<uint16_t>(value & 1U);
        value = u16((value >> 1U) | (value & 0x8000U));
        return carry;
    }

    uint16_t rcr1(uint16_t &value, const uint16_t carryIn)
    {
        const auto carry = static_cast<uint16_t>(value & 1U);
        value = u16((value >> 1U) | ((carryIn & 1U) << 15U));
        return carry;
    }

    uint16_t add16(uint16_t &a, const uint16_t b)
    {
        const auto sum = static_cast<uint32_t>(a) + b;
        a = u16(sum);
        return static_cast<uint16_t>(sum > 0xffffU);
    }

    uint16_t adc16(uint16_t &a, const uint16_t b, const uint16_t carryIn)
    {
        const auto sum = static_cast<uint32_t>(a) + b + (carryIn & 1U);
        a = u16(sum);
        return static_cast<uint16_t>(sum > 0xffffU);
    }

    uint16_t sub16(uint16_t &a, const uint16_t b)
    {
        const auto carry = static_cast<uint16_t>(a < b);
        a = u16(static_cast<uint32_t>(a) - b);
        return carry;
    }

    uint16_t sbb16(uint16_t &a, const uint16_t b, const uint16_t carryIn)
    {
        const auto rhs = static_cast<uint32_t>(b) + (carryIn & 1U);
        const auto carry = static_cast<uint16_t>(a < rhs);
        a = u16(static_cast<uint32_t>(a) - rhs);
        return carry;
    }

    void leftShift48(uint16_t &bx, uint16_t &ax, uint16_t &dx, const int count)
    {
        uint16_t carry = 0;
        for (int i = 0; i < count; ++i)
        {
            carry = shl1(bx);
            carry = rcl1(ax, carry);
            carry = rcl1(dx, carry);
        }
    }

    void arithmeticRightShift32(uint16_t &hi, uint16_t &lo, const int count)
    {
        uint16_t carry = 0;
        for (int i = 0; i < count; ++i)
        {
            carry = sar1(hi);
            carry = rcr1(lo, carry);
        }
    }

    enum class TransformOp
    {
        Add,
        Subtract,
        ShiftOnly,
    };

    struct TransformStep
    {
        TransformOp op;
        int shift;
    };

    std::pair<uint16_t, uint16_t>
    transform(const uint16_t lo, const uint16_t hi,
              const std::initializer_list<TransformStep> steps)
    {
        auto bx = lo;
        auto ax = hi;
        auto dx = cwd(ax);
        const auto sign = dx;

        for (const auto &step : steps)
        {
            leftShift48(bx, ax, dx, step.shift);

            if (step.op == TransformOp::Add)
            {
                auto carry = add16(bx, lo);
                carry = adc16(ax, hi, carry);
                adc16(dx, sign, carry);
            }
            else if (step.op == TransformOp::Subtract)
            {
                auto carry = sub16(bx, lo);
                carry = sbb16(ax, hi, carry);
                sbb16(dx, sign, carry);
            }
        }

        arithmeticRightShift32(dx, ax, 4);
        return {ax, dx};
    }

    class Mpc60SetSampleConverter
    {
    public:
        int16_t convert(const uint64_t sampleWord, const bool isOddWordIndex)
        {
            const auto inputWord =
                toFirmwareInputWord(sampleWord, isOddWordIndex);

            const auto oldLo = stateLo;
            const auto oldHi = stateHi;

            const auto a = transform(oldLo, oldHi,
                                     {{TransformOp::Add, 2},
                                      {TransformOp::Subtract, 4},
                                      {TransformOp::Add, 4},
                                      {TransformOp::Add, 4},
                                      {TransformOp::Add, 2},
                                      {TransformOp::Subtract, 3}});

            auto newLo = oldLo;
            auto newHi = inputWord;
            arithmeticRightShift32(newHi, newLo, 8);
            auto carry = add16(newLo, a.first);
            adc16(newHi, a.second, carry);

            const auto b = transform(newLo, newHi,
                                     {{TransformOp::Add, 1},
                                      {TransformOp::Subtract, 3},
                                      {TransformOp::Subtract, 3},
                                      {TransformOp::Subtract, 3},
                                      {TransformOp::Add, 5},
                                      {TransformOp::ShiftOnly, 3}});

            const auto c = transform(oldLo, oldHi,
                                     {{TransformOp::Subtract, 2},
                                      {TransformOp::Add, 3},
                                      {TransformOp::Add, 4},
                                      {TransformOp::Add, 1},
                                      {TransformOp::Subtract, 3},
                                      {TransformOp::Add, 2}});

            auto mixedLo = b.first;
            auto mixedHi = b.second;
            carry = add16(mixedLo, c.first);
            adc16(mixedHi, c.second, carry);

            stateLo = newLo;
            stateHi = newHi;

            auto tmpLo = mixedLo;
            auto tmpHi = mixedHi;
            arithmeticRightShift32(tmpHi, tmpLo, 3);

            auto outLo = mixedLo;
            auto outHi = mixedHi;
            carry = add16(outLo, tmpLo);
            carry = adc16(outHi, tmpHi, carry);

            arithmeticRightShift32(tmpHi, tmpLo, 1);
            carry = add16(outLo, tmpLo);
            adc16(outHi, tmpHi, carry);

            return scaleAndClip(outLo, outHi);
        }

    private:
        uint16_t stateLo = 0;
        uint16_t stateHi = 0;

        // Generated read_bits_int_le(12) values preserve the firmware bytes,
        // but the second word in each 3-byte pair has the shared nibble in the
        // low bits. Normalize to the input word used by MPC2000XL firmware
        // routine 35b0:1eac before applying the fixed-point converter.
        static uint16_t toFirmwareInputWord(const uint64_t sampleWord,
                                            const bool isOddWordIndex)
        {
            if (isOddWordIndex)
            {
                return static_cast<uint16_t>((sampleWord & 0x0fffU) << 4U);
            }

            return static_cast<uint16_t>(((sampleWord & 0x00ffU) << 8U) |
                                         ((sampleWord & 0x0f00U) >> 4U));
        }

        static int16_t scaleAndClip(uint16_t lo, uint16_t hi)
        {
            const auto negative = (hi & 0x8000U) != 0;
            if (!negative)
            {
                if ((hi >> 8U) != 0)
                {
                    return static_cast<int16_t>(kMpc2000xlSetImportScale - 1);
                }

                const auto magnitude = static_cast<uint16_t>(
                    ((hi & 0x00ffU) << 8U) | ((lo >> 8U) & 0x00ffU));
                if (magnitude >= kMpc2000xlSetImportScale)
                {
                    return static_cast<int16_t>(kMpc2000xlSetImportScale - 1);
                }
            }
            else
            {
                if ((hi >> 8U) != 0x00ffU)
                {
                    return static_cast<int16_t>(
                        -static_cast<int>(kMpc2000xlSetImportScale - 1));
                }

                const auto magnitude = static_cast<uint16_t>(
                    -static_cast<int16_t>(static_cast<uint16_t>(
                        ((hi & 0x00ffU) << 8U) | ((lo >> 8U) & 0x00ffU))));
                if (magnitude >= kMpc2000xlSetImportScale)
                {
                    return static_cast<int16_t>(
                        -static_cast<int>(kMpc2000xlSetImportScale - 1));
                }
            }

            for (int i = 0; i < 7; ++i)
            {
                const auto carry = shl1(lo);
                rcl1(hi, carry);
            }

            const auto dividend = static_cast<int32_t>(s16(hi)) * 65536 + lo;
            return static_cast<int16_t>(dividend / kMpc2000xlSetImportScale);
        }
    };

    float sampleWordToFloat(Mpc60SetSampleConverter &converter,
                            const uint64_t sampleWord,
                            const bool isOddWordIndex)
    {
        return mpc::sampleops::short_to_float(
            converter.convert(sampleWord, isOddWordIndex));
    }

    sound_or_error loadEntry(mpc60_set_v1_t &parsed,
                             const size_t soundDirectoryEntryIndex,
                             const std::shared_ptr<mpc::sampler::Sound> &sound)
    {
        const auto *entries = parsed.sound_directory_entry();
        if (soundDirectoryEntryIndex >= entries->size())
        {
            return tl::make_unexpected("SET sound entry index out of range");
        }

        const auto &entry = entries->at(soundDirectoryEntryIndex);
        const auto start =
            static_cast<size_t>(entry->start_address_in_memory());
        const auto length = static_cast<size_t>(entry->length_in_samples());
        const auto *sampleWords = parsed.sound_samples();

        if (start > sampleWords->size() || length > sampleWords->size() - start)
        {
            return tl::make_unexpected("SET sound sample range out of bounds");
        }

        sound->setName(trimRightSpaces(entry->name()));
        sound->setSampleRate(kMpc60SampleRate);
        sound->setMono(true);
        sound->setLevel(100);
        sound->setTune(0);
        sound->setLoopEnabled(false);

        auto sampleData = sound->getMutableSampleData();
        sampleData->clear();
        sampleData->reserve(length);

        Mpc60SetSampleConverter converter;
        for (size_t i = 0; i < length; ++i)
        {
            sampleData->push_back(sampleWordToFloat(converter,
                                                    sampleWords->at(start + i),
                                                    ((start + i) % 2U) != 0));
        }

        sound->setStart(0);
        sound->setEnd(sound->getLastFrameIndex());
        sound->setLoopTo(0);

        return sound;
    }
} // namespace

sound_or_error mpc::file::kaitai::Mpc60SetSoundLoader::loadSoundDirectoryEntry(
    const std::shared_ptr<mpc::disk::MpcFile> &file,
    const size_t soundDirectoryEntryIndex,
    const std::shared_ptr<mpc::sampler::Sound> &sound)
{
    return loadSoundDirectoryEntry(file->getBytes(), soundDirectoryEntryIndex,
                                   sound);
}

sound_or_error mpc::file::kaitai::Mpc60SetSoundLoader::loadSoundDirectoryEntry(
    const std::vector<char> &bytes, const size_t soundDirectoryEntryIndex,
    const std::shared_ptr<mpc::sampler::Sound> &sound)
{
    std::stringstream parseStream(std::string(bytes.begin(), bytes.end()),
                                  std::ios::in | std::ios::out |
                                      std::ios::binary);
    ::kaitai::kstream parseIo(&parseStream);
    mpc60_set_v1_t parsed(&parseIo);
    parsed._read();

    return loadEntry(parsed, soundDirectoryEntryIndex, sound);
}

sound_or_error
mpc::file::kaitai::Mpc60SetSoundLoader::loadAssignedSoundAtMpc60Pad(
    const std::shared_ptr<mpc::disk::MpcFile> &file, const size_t mpc60PadIndex,
    const std::shared_ptr<mpc::sampler::Sound> &sound)
{
    return loadAssignedSoundAtMpc60Pad(file->getBytes(), mpc60PadIndex, sound);
}

sound_or_error
mpc::file::kaitai::Mpc60SetSoundLoader::loadAssignedSoundAtMpc60Pad(
    const std::vector<char> &bytes, const size_t mpc60PadIndex,
    const std::shared_ptr<mpc::sampler::Sound> &sound)
{
    std::stringstream parseStream(std::string(bytes.begin(), bytes.end()),
                                  std::ios::in | std::ios::out |
                                      std::ios::binary);
    ::kaitai::kstream parseIo(&parseStream);
    mpc60_set_v1_t parsed(&parseIo);
    parsed._read();

    const auto *soundMap = parsed.sound_map();
    if (mpc60PadIndex >= soundMap->size())
    {
        return tl::make_unexpected("SET MPC60 pad index out of range");
    }

    const auto entryIndex = soundMap->at(mpc60PadIndex);
    const auto *entries = parsed.sound_directory_entry();
    if (entryIndex >= entries->size())
    {
        return tl::make_unexpected("SET sound entry index out of range");
    }

    return loadEntry(parsed, entryIndex, sound);
}
