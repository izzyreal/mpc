#pragma once

#include "mpc_fs.hpp"
#include "input/midi/legacy/MidiControlCommand.hpp"

#include <string>
#include <vector>
#include <memory>

namespace mpc
{
    class Mpc;
}

namespace mpc::input::midi::legacy
{
    struct MidiControlPreset
    {
        enum AutoLoadMode
        {
            AutoLoadModeNo,
            AutoLoadModeAsk,
            AutoLoadModeYes
        };
        std::string name;
        unsigned char autoloadMode = AutoLoadModeAsk;
        std::vector<MidiControlCommand> rows;
    };

    class MidiControlPersistence
    {
        static void saveVmpcMidiScreenPresetToFile(Mpc &mpc, const fs::path &p,
                                                   const std::string &name);
        static void healPreset(Mpc &,
                               const std::shared_ptr<MidiControlPreset> &);

    public:
        static bool doesPresetWithNameExist(Mpc &, std::string name);

        static void
        loadFileByNameIntoPreset(Mpc &, const std::string &name,
                                 const std::shared_ptr<MidiControlPreset> &);
        static void loadAllPresetsFromDiskIntoMemory(Mpc &);

        static std::shared_ptr<MidiControlPreset> createDefaultPreset(Mpc &);
        static void loadDefaultMapping(Mpc &mpc);

        // Persistence of the in-memory mapping, so it's not required
        // to save a preset explicitly before restarting the application.
        static void restoreLastState(Mpc &mpc);
        static void saveCurrentState(Mpc &mpc);
    };
} // namespace mpc::nvram
