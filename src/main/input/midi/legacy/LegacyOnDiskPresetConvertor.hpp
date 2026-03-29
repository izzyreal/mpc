#pragma once

#include "FileIoPolicy.hpp"
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
        using namespace mpc::file_io;

        const auto jsonFilePath =
            newPath.value_or(mpc_fs::path(p).replace_extension(".json"));

        const auto existsValue =
            value(mpc_fs::exists(p), FailurePolicy::Recoverable,
                  "legacy MIDI preset conversion preflight");
        if (!existsValue || !*existsValue)
        {
            return;
        }

        const auto isRegularFileValue =
            value(mpc_fs::is_regular_file(p), FailurePolicy::Recoverable,
                  "legacy MIDI preset conversion preflight");
        if (!isRegularFileValue || !*isRegularFileValue ||
            p.extension().string().find("vmp") == std::string::npos)
        {
            return;
        }

        const auto jsonExistsValue =
            value(mpc_fs::exists(jsonFilePath), FailurePolicy::Recoverable,
                  "legacy MIDI preset conversion preflight");
        if (!jsonExistsValue || *jsonExistsValue)
        {
            return;
        }

        const auto fileData =
            value(get_file_data(p), FailurePolicy::Recoverable,
                  "legacy MIDI preset conversion input read");
        if (!fileData)
        {
            return;
        }

        json fileAsJson;

        bool parseSucceeded = false;

        bool shouldTryV1Parser = true;

        try
        {
            fileAsJson = parseLegacyMidiControlPresetV2(
                std::string(fileData->begin(), fileData->end()));
            shouldTryV1Parser = false;
            parseSucceeded = true;
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
                parseSucceeded = true;
            }
            catch (const std::exception &)
            {
            }
        }

        if (!parseSucceeded)
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

        if (!success(set_file_data(jsonFilePath, fileAsJson.dump(4)),
                     FailurePolicy::Recoverable,
                     "legacy MIDI preset conversion output write"))
        {
            return;
        }

        auto newFilePath = p;

        newFilePath += ".bk";

        (void) success(mpc_fs::rename(p, newFilePath),
                       FailurePolicy::Recoverable,
                       "legacy MIDI preset backup rename");
    }

    inline void convertOnDiskLegacyPresets(const mpc_fs::path &p)
    {
        using namespace mpc::file_io;

        const auto dir_it_res =
            value(mpc_fs::make_directory_iterator(p),
                  FailurePolicy::Recoverable,
                  "legacy MIDI preset conversion directory scan");
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
