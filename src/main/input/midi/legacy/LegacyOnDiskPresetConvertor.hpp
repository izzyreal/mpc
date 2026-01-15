#pragma once

#include "input/midi/MidiControlPresetUtil.hpp"
#include "input/midi/legacy/LegacyMidiControlPresetV1Convertor.hpp"
#include "input/midi/legacy/LegacyMidiControlPresetV2Convertor.hpp"
#include "input/midi/legacy/LegacyMidiControlPresetPatcher.hpp"

#include "mpc_fs.hpp"

namespace mpc::input::midi::legacy
{
    inline void convertOnDiskLegacyPresets(const fs::path &p)
    {
        for (const auto &f : fs::directory_iterator(p))
        {
            if (!fs::is_regular_file(f) ||
                f.path().extension().string().find("vmp") == std::string::npos)
            {
                continue;
            }

            const auto fileData = get_file_data(f.path());

            json fileAsJson;

            bool success = false;

            bool shouldTryV1Parser = true;

            try
            {
                fileAsJson = parseLegacyMidiControlPresetV2(
                    std::string(fileData.begin(), fileData.end()));
                shouldTryV1Parser = false;
                success = true;
            }
            catch (const std::exception &)
            {
            }

            if (shouldTryV1Parser)
            {
                try
                {
                    fileAsJson = parseLegacyMidiControlPresetV1(
                        std::string(fileData.begin(), fileData.end()));
                    success = true;
                }
                catch (const std::exception &)
                {
                }
            }

            if (!success)
            {
                continue;
            }

            auto preset = std::make_shared<MidiControlPresetV3>();

            try
            {
                from_json(fileAsJson, *preset);
            }
            catch (const std::exception)
            {
                continue;
            }

            patchLegacyPreset(fileAsJson, MidiControlPresetUtil::load_schema());

            MidiControlPresetUtil::ensurePresetHasAllAvailableTargets(preset);

            MidiControlPresetUtil::ensureTargetsAreInSameOrderAsInSchema(
                preset);

            to_json(fileAsJson, *preset);

            auto jsonFilePath = f.path();

            jsonFilePath.replace_extension(".json");

            set_file_data(jsonFilePath, fileAsJson.dump(4));

            auto newFilePath = f.path();

            newFilePath += ".bk";

            fs::rename(f.path(), newFilePath);
        }
    }
} // namespace mpc::input::midi::legacy