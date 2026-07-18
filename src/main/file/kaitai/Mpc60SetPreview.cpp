#include "Mpc60SetPreview.hpp"

#include "disk/MpcFile.hpp"
#include "file/kaitai/generated/mpc60_set_v0.h"
#include "file/kaitai/generated/mpc60_set_v1.h"

#include <kaitai/kaitaistream.h>

#include <array>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string_view>

namespace
{
    constexpr uint8_t kNoSoundDirectoryEntry = 255;

    std::string trimRightSpaces(std::string value)
    {
        while (!value.empty() && value.back() == ' ')
        {
            value.pop_back();
        }
        return value;
    }

    constexpr std::array<std::string_view, 34> kMpc60PadNames{
        "hiht_clsd", "hiht_medm", "hiht_open", "snr1", "snr2", "bass",
        "tom1",      "tom2",      "tom3",      "tom4", "rid1", "rid2",
        "crs1",      "crs2",      "prc1",      "prc2", "prc3", "prc4",
        "dr01",      "dr02",      "dr03",      "dr04", "dr05", "dr06",
        "dr07",      "dr08",      "dr09",      "dr10", "dr11", "dr12",
        "dr13",      "dr14",      "dr15",      "dr16",
    };

    std::string formatSourceSlotLabel(const char bank, const int slotNumber)
    {
        std::ostringstream oss;
        oss << bank << std::setw(2) << std::setfill('0') << slotNumber;
        return oss.str();
    }

    template <typename ParsedSet>
    mpc::file::kaitai::Mpc60SetPreview loadParsedPreview(ParsedSet &parsed)
    {
        using mpc::file::kaitai::Mpc60SetPreview;
        using mpc::file::kaitai::Mpc60SetPreviewEntry;

        Mpc60SetPreview result;
        result.totalNumberOfSampleWords =
            parsed.total_number_of_sample_words()->value();
        result.useMasterMixData =
            static_cast<int>(parsed.use_master_mix_data()) == 1;
        const auto masterStereoMix = parsed.master_stereo_mix();
        const auto masterStereoPan = parsed.master_stereo_pan();
        result.masterStereoMix.assign(masterStereoMix.begin(),
                                      masterStereoMix.end());
        result.masterStereoPan.assign(masterStereoPan.begin(),
                                      masterStereoPan.end());

        result.soundDirectoryEntries.reserve(
            parsed.sound_directory_entry()->size());
        for (const auto &entry : *parsed.sound_directory_entry())
        {
            Mpc60SetPreviewEntry previewEntry;
            previewEntry.name = trimRightSpaces(entry->name());
            previewEntry.sharedSoundLink = entry->shared_sound_link();
            previewEntry.lengthInSamples = entry->length_in_samples();
            previewEntry.startAddressInMemory =
                entry->start_address_in_memory();
            previewEntry.startAddressForPlaying =
                entry->start_address_for_playing();
            previewEntry.soundDuration = entry->sound_duration();
            previewEntry.requestedStereoMixVolume =
                entry->requested_stereo_mix_volume();
            previewEntry.requestedStereoMixPan = entry->requested_stereo_mix_pan();
            previewEntry.pitchFactor = entry->pitch_factor();
            previewEntry.isHihat =
                static_cast<int>(
                    entry->sound_characteristics()->normal_or_hihat_sound()) ==
                1;
            result.soundDirectoryEntries.push_back(std::move(previewEntry));
        }

        result.soundDirectoryEntryIndexByMpc60Pad.reserve(
            parsed.sound_map()->size());
        for (const auto entryIndex : *parsed.sound_map())
        {
            if (entryIndex != kNoSoundDirectoryEntry &&
                entryIndex >= result.soundDirectoryEntries.size())
            {
                throw std::runtime_error(
                    "MPC60 SET sound_map entry index out of range");
            }
            result.soundDirectoryEntryIndexByMpc60Pad.push_back(entryIndex);
        }

        const auto parsedSampleWords = parsed.sound_samples();
        if (parsedSampleWords->size() < result.totalNumberOfSampleWords)
        {
            throw std::runtime_error("MPC60 SET sample data is truncated");
        }

        auto sampleWords = std::make_shared<std::vector<uint16_t>>();
        sampleWords->reserve(result.totalNumberOfSampleWords);
        for (size_t i = 0; i < result.totalNumberOfSampleWords; ++i)
        {
            const auto value = parsedSampleWords->at(i);
            if (value > 0x0fffU)
            {
                throw std::runtime_error(
                    "MPC60 SET sample word is out of range");
            }
            sampleWords->push_back(static_cast<uint16_t>(value));
        }
        result.soundSampleWords = std::move(sampleWords);

        return result;
    }
}

const mpc::file::kaitai::Mpc60SetPreviewEntry *
mpc::file::kaitai::Mpc60SetPreview::assignedSoundAtMpc60Pad(
    const size_t padIndex) const
{
    if (padIndex >= soundDirectoryEntryIndexByMpc60Pad.size())
    {
        return nullptr;
    }

    const auto soundIndex = soundDirectoryEntryIndexByMpc60Pad[padIndex];
    if (soundIndex >= soundDirectoryEntries.size())
    {
        return nullptr;
    }

    return &soundDirectoryEntries[soundIndex];
}

std::string
mpc::file::kaitai::Mpc60SetPreview::mpc60PadName(const size_t padIndex)
{
    if (padIndex >= kMpc60PadNames.size())
    {
        return {};
    }

    return std::string(kMpc60PadNames[padIndex]);
}

std::string
mpc::file::kaitai::Mpc60SetPreview::mpc60SourceSlotLabel(
    const size_t padIndex)
{
    if (padIndex <= 2)
    {
        return formatSourceSlotLabel('A', 1);
    }

    if (padIndex <= 17)
    {
        return formatSourceSlotLabel('A', static_cast<int>(padIndex) - 1);
    }

    if (padIndex <= 33)
    {
        return formatSourceSlotLabel('B', static_cast<int>(padIndex) - 17);
    }

    return {};
}

mpc::file::kaitai::Mpc60SetPreview
mpc::file::kaitai::Mpc60SetPreviewLoader::loadPreview(
    const std::shared_ptr<mpc::disk::MpcFile> &file)
{
    return loadPreview(file->getBytes());
}

mpc::file::kaitai::Mpc60SetPreview
mpc::file::kaitai::Mpc60SetPreviewLoader::loadPreview(
    const std::vector<char> &bytes)
{
    if (bytes.size() < 2 || static_cast<uint8_t>(bytes[0]) != 0x02)
    {
        throw std::runtime_error("unsupported MPC60 SET file signature");
    }

    std::istringstream parseStream(std::string(bytes.begin(), bytes.end()),
                                   std::ios::in | std::ios::binary);
    ::kaitai::kstream parseIo(&parseStream);

    const auto version = static_cast<uint8_t>(bytes[1]);
    if (version == 0x00)
    {
        mpc60_set_v0_t parsed(&parseIo);
        parsed._read();
        return loadParsedPreview(parsed);
    }

    if (version == 0x01)
    {
        mpc60_set_v1_t parsed(&parseIo);
        parsed._read();
        return loadParsedPreview(parsed);
    }

    throw std::runtime_error("unsupported MPC60 SET file version");
}
