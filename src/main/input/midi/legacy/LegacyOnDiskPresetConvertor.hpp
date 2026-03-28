#pragma once

#include "input/midi/MidiControlPresetUtil.hpp"
#include "input/midi/legacy/LegacyMidiControlPresetV1Convertor.hpp"
#include "input/midi/legacy/LegacyMidiControlPresetV2Convertor.hpp"
#include "input/midi/legacy/LegacyMidiControlPresetPatcher.hpp"

#include "mpc_fs.hpp"

namespace mpc::input::midi::legacy
{
    inline void convertOnDiskLegacyPreset(
        const mpc_fs::path &p,
        const std::optional<mpc_fs::path> &newPath = std::nullopt)
    {
        const auto jsonFilePath =
            newPath.value_or(mpc_fs::path(p).replace_extension(".json"));

        if (!mpc_fs::exists(p).value_or(false) || !mpc_fs::is_regular_file(p).value_or(false) ||
            p.extension().string().find("vmp") == std::string::npos ||
            mpc_fs::exists(jsonFilePath).value_or(false))
        {
            return;
        }

        const auto fileData = get_file_data(p);
        if (!fileData)
        {
            return;
        }

        json fileAsJson;

        bool success = false;

        bool shouldTryV1Parser = true;

        try
        {
            fileAsJson = parseLegacyMidiControlPresetV2(
                std::string(fileData->begin(), fileData->end()));
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
                    std::string(fileData->begin(), fileData->end()));
                success = true;
            }
            catch (const std::exception &)
            {
            }
        }

        if (!success)
        {
            return;
        }

        const auto preset = std::make_shared<MidiControlPresetV3>();

        try
        {
            from_json(fileAsJson, *preset);
        }
        catch (const std::exception)
        {
            return;
        }

        patchLegacyPreset(fileAsJson, MidiControlPresetUtil::load_schema());

        MidiControlPresetUtil::ensurePresetHasAllAvailableTargets(preset);

        MidiControlPresetUtil::ensureTargetsAreInSameOrderAsInSchema(preset);

        to_json(fileAsJson, *preset);

        if (!set_file_data(jsonFilePath, fileAsJson.dump(4)))
        {
            return;
        }

        auto newFilePath = p;

        newFilePath += ".bk";

        (void) mpc_fs::rename(p, newFilePath);
    }

    inline void convertOnDiskLegacyPresets(const mpc_fs::path &p)
    {
        const auto dir_it_res = mpc_fs::make_directory_iterator(p);
        if (!dir_it_res)
        {
            return;
        }

        for (auto f = *dir_it_res; f != mpc_fs::directory_end(); ++f)
        {
            convertOnDiskLegacyPreset(f->path());
        }
    }
} // namespace mpc::input::midi::legacy
