#pragma once

#include "Mpc.hpp"
#include "mpc_fs.hpp"

#include <string>
#include <vector>
#include <memory>

namespace mpc::nvram {
    struct MidiControlCommand {

        std::string label;
        bool isNote = false;
        char channel = -2;
        char value = -2;
        bool isEmpty() {
            return !isNote && channel == -2 && value == -2;
        }
        void reset() {
            isNote = false; channel = -2; value = -2;
        }
        MidiControlCommand() {}
        MidiControlCommand(std::string newLabel, bool newIsNote, int newChannelIndex, int newValue);
        MidiControlCommand(const MidiControlCommand& c);
        bool equals(const MidiControlCommand& other);

        std::vector<char> toBytes();
    };

    struct MidiControlPreset {
        enum AutoLoadMode { NO, ASK, YES };
        std::string name;
        unsigned char autoloadMode = AutoLoadMode::ASK;
        std::vector<MidiControlCommand> rows;
    };

    class MidiControlPersistence
    {
    private:
        static void saveVmpcMidiScreenPresetToFile(mpc::Mpc& mpc, fs::path p, std::string name);

    public:
        static bool doesPresetWithNameExist(std::string name);
        static void deleteLastState();
        static void loadFileByNameIntoPreset(mpc::Mpc&, std::string name, std::shared_ptr<MidiControlPreset>);
        static void loadAllPresetsFromDiskIntoMemory(mpc::Mpc&);

        static void loadDefaultMapping(Mpc &mpc);

        // Persistence of the in-memory mapping, so it's not required
        // to save a preset explicitly before restarting the application.
        static void restoreLastState(mpc::Mpc& mpc);
        static void saveCurrentState(mpc::Mpc& mpc);
    };
}