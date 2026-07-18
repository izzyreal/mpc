#include "file/kaitai/Mpc60SetSoundLoader.hpp"

#include "disk/MpcFile.hpp"
#include "file/kaitai/Mpc60SampleDecoder.hpp"
#include "file/kaitai/Mpc60SetPreview.hpp"
#include "sampler/Sound.hpp"

namespace
{
    std::string validateSetHeader(const std::vector<char> &bytes)
    {
        if (bytes.size() < 2 || static_cast<uint8_t>(bytes[0]) != 0x02)
        {
            return "unsupported MPC60 SET file signature";
        }

        const auto version = static_cast<uint8_t>(bytes[1]);
        if (version != 0x00 && version != 0x01)
        {
            return "unsupported MPC60 SET file version";
        }

        return {};
    }

    std::string trimRightSpaces(std::string value)
    {
        while (!value.empty() && value.back() == ' ')
        {
            value.pop_back();
        }
        return value;
    }
    sound_or_error loadEntry(const mpc::file::kaitai::Mpc60SetPreview &preview,
                             const size_t soundDirectoryEntryIndex,
                             const std::shared_ptr<mpc::sampler::Sound> &sound)
    {
        if (soundDirectoryEntryIndex >= preview.soundDirectoryEntries.size())
        {
            return tl::make_unexpected("SET sound entry index out of range");
        }

        const auto &entry =
            preview.soundDirectoryEntries[soundDirectoryEntryIndex];
        const auto start = static_cast<size_t>(entry.startAddressInMemory);
        const auto length = static_cast<size_t>(entry.lengthInSamples);
        const auto sampleWords = preview.soundSampleWords;

        if (!sampleWords || start > sampleWords->size() ||
            length > sampleWords->size() - start)
        {
            return tl::make_unexpected("SET sound sample range out of bounds");
        }

        sound->setName(trimRightSpaces(entry.name));
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
    if (const auto error = validateSetHeader(bytes); !error.empty())
    {
        return tl::make_unexpected(error);
    }

    const auto preview =
        mpc::file::kaitai::Mpc60SetPreviewLoader::loadPreview(bytes);
    return loadSoundDirectoryEntry(preview, soundDirectoryEntryIndex, sound);
}

sound_or_error mpc::file::kaitai::Mpc60SetSoundLoader::loadSoundDirectoryEntry(
    const Mpc60SetPreview &preview, const size_t soundDirectoryEntryIndex,
    const std::shared_ptr<mpc::sampler::Sound> &sound)
{
    return loadEntry(preview, soundDirectoryEntryIndex, sound);
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
    if (const auto error = validateSetHeader(bytes); !error.empty())
    {
        return tl::make_unexpected(error);
    }

    const auto preview =
        mpc::file::kaitai::Mpc60SetPreviewLoader::loadPreview(bytes);
    return loadAssignedSoundAtMpc60Pad(preview, mpc60PadIndex, sound);
}

sound_or_error
mpc::file::kaitai::Mpc60SetSoundLoader::loadAssignedSoundAtMpc60Pad(
    const Mpc60SetPreview &preview, const size_t mpc60PadIndex,
    const std::shared_ptr<mpc::sampler::Sound> &sound)
{
    if (mpc60PadIndex >= preview.soundDirectoryEntryIndexByMpc60Pad.size())
    {
        return tl::make_unexpected("SET MPC60 pad index out of range");
    }

    const auto entryIndex =
        preview.soundDirectoryEntryIndexByMpc60Pad[mpc60PadIndex];
    return loadEntry(preview, entryIndex, sound);
}
