#pragma once

#include "mpc_fs.hpp"
#include "MidiControlCommand.hpp"

#include <string>
#include <vector>
#include <memory>

namespace mpc
{
    class Mpc;
}

namespace mpc::nvram
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
        unsigned char autoloadMode = AutoLoadMode::AutoLoadModeAsk;
        std::vector<MidiControlCommand> rows;
    };

    class MidiControlPersistence
    {
    private:
        static void saveVmpcMidiScreenPresetToFile(mpc::Mpc &mpc,
                                                   const fs::path &p,
                                                   const std::string &name);
        static void healPreset(mpc::Mpc &,
                               const std::shared_ptr<MidiControlPreset> &);

    public:
        static bool doesPresetWithNameExist(mpc::Mpc &, std::string name);

        static void
        loadFileByNameIntoPreset(mpc::Mpc &, const std::string &name,
                                 const std::shared_ptr<MidiControlPreset> &);
        static void loadAllPresetsFromDiskIntoMemory(mpc::Mpc &);

        static std::shared_ptr<MidiControlPreset>
        createDefaultPreset(mpc::Mpc &);
        static void loadDefaultMapping(Mpc &mpc);

        // Persistence of the in-memory mapping, so it's not required
        // to save a preset explicitly before restarting the application.
        static void restoreLastState(mpc::Mpc &mpc);
        static void saveCurrentState(mpc::Mpc &mpc);
    };
} // namespace mpc::nvram
