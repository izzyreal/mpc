#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace mpc::disk
{
    class MpcFile;
}

namespace mpc::file::kaitai
{
    struct Mpc60SetPreviewEntry
    {
        std::string name;
        uint8_t sharedSoundLink = 0;
        uint32_t lengthInSamples = 0;
        uint32_t startAddressForPlaying = 0;
        uint16_t soundDuration = 0;
        uint8_t requestedStereoMixVolume = 0;
        uint8_t requestedStereoMixPan = 0;
        uint16_t pitchFactor = 4096;
        bool isHihat = false;
    };

    struct Mpc60SetPreview
    {
        uint32_t totalNumberOfSampleWords = 0;
        bool useMasterMixData = false;
        std::vector<uint8_t> masterStereoMix;
        std::vector<uint8_t> masterStereoPan;
        std::vector<Mpc60SetPreviewEntry> soundDirectoryEntries;
        std::vector<uint8_t> soundDirectoryEntryIndexByMpc60Pad;

        const Mpc60SetPreviewEntry *assignedSoundAtMpc60Pad(size_t padIndex) const;

        static std::string mpc60PadName(size_t padIndex);
        static std::string mpc60SourceSlotLabel(size_t padIndex);
    };

    class Mpc60SetPreviewLoader
    {
    public:
        static Mpc60SetPreview
        loadPreview(const std::shared_ptr<disk::MpcFile> &file);

        static Mpc60SetPreview loadPreview(const std::vector<char> &bytes);
    };
}
