#include "file/kaitai/Mpc60SetSoundLoader.hpp"

#include "disk/MpcFile.hpp"
#include "file/kaitai/Mpc60SampleDecoder.hpp"
#include "file/kaitai/generated/mpc60_set_v0.h"
#include "sampler/Sound.hpp"

#include <kaitai/kaitaistream.h>

#include <sstream>

namespace
{
    std::string trimRightSpaces(std::string value)
    {
        while (!value.empty() && value.back() == ' ')
        {
            value.pop_back();
        }
        return value;
    }
    sound_or_error loadEntry(mpc60_set_v0_t &parsed,
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
        sound->setSampleRate(44100);
        sound->setMono(true);
        sound->setLevel(100);
        sound->setTune(-17);
        sound->setLoopEnabled(false);

        auto sampleData = sound->getMutableSampleData();
        sampleData->clear();
        sampleData->reserve(length);

        mpc::file::kaitai::Mpc60SampleDecoder converter;
        for (size_t i = 0; i < length; ++i)
        {
            sampleData->push_back(converter.decodeImportedFloat(
                static_cast<uint16_t>(sampleWords->at(start + i)),
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
    mpc60_set_v0_t parsed(&parseIo);
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
    mpc60_set_v0_t parsed(&parseIo);
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
