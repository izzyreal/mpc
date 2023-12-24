#pragma once

#include "mpc_fs.hpp"
#include "MidiControlCommand.hpp"

#include <string>
#include <vector>
#include <memory>

namespace mpc { class Mpc; }

namespace mpc::nvram {
    struct MidiControlPreset {
        enum AutoLoadMode { AutoLoadModeNo, AutoLoadModeAsk, AutoLoadModeYes };
        std::string name;
        unsigned char autoloadMode = AutoLoadMode::AutoLoadModeAsk;
        std::vector<MidiControlCommand> rows;
    };

    class MidiControlPersistence
    {
    private:
        static void saveVmpcMidiScreenPresetToFile(mpc::Mpc& mpc, fs::path p, std::string name);

    public:
        static bool doesPresetWithNameExist(mpc::Mpc&, std::string name);

        static void loadFileByNameIntoPreset(mpc::Mpc&, std::string name, std::shared_ptr<MidiControlPreset>);
        static void loadAllPresetsFromDiskIntoMemory(mpc::Mpc&);

        static void loadDefaultMapping(Mpc &mpc);

        // Persistence of the in-memory mapping, so it's not required
        // to save a preset explicitly before restarting the application.
        static void restoreLastState(mpc::Mpc& mpc);
        static void saveCurrentState(mpc::Mpc& mpc);
    };
}