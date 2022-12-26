#pragma once

#include "Mpc.hpp"

#include <string>
#include <vector>

namespace moduru::file { class File; }

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
        MidiControlCommand(std::string newLabel, bool newIsNote, int newChannelIndex, int newValue) : label(newLabel), isNote(newIsNote), channel(newChannelIndex), value(newValue) {}
        MidiControlCommand(const MidiControlCommand& c) : label(c.label), isNote(c.isNote), channel(c.channel), value(c.value) {}
        bool equals(const MidiControlCommand& other) {
            return isNote == other.isNote && channel == other.channel && value == other.value;
        }

        std::vector<char> toBytes() {
            std::vector<char> result;

            for (int i = 0; i < label.size(); i++)
            {
                result.push_back(label[i]);
            }

            result.push_back(' ');

            result.push_back(isNote ? 1 : 0);
            result.push_back(channel);
            result.push_back(value);
            return result;
        }
    };

    struct MidiControlPreset {
        enum AutoLoadMode { NO, ASK, YES };
        std::string name = "";
        unsigned char autoloadMode = AutoLoadMode::ASK;
        std::vector<MidiControlCommand> rows;
    };

    class MidiControlPersistence
    {
    private:
        static void saveVmpcMidiScreenPresetToFile(mpc::Mpc& mpc, moduru::file::File &f, std::string name, int autoLoadMode);

    public:
        static void deleteLastState();
        static void loadFileIntoPreset(moduru::file::File&, MidiControlPreset&);
        static void loadFileByNameIntoPreset(std::string name, MidiControlPreset&);
        static void loadAllPresetsFromDiskIntoMemory();
        static void savePresetToFile(MidiControlPreset &preset);

        static void loadDefaultMapping(Mpc &mpc);

        // Presets available in the default preset path,
        // loaded into memory, and maintained as presets are changed, added or deleted.
        static std::vector<MidiControlPreset> presets;

        // Persistence of the in-memory mapping, so it's not required
        // to save a preset explicitly before restarting the application.
        static void restoreLastState(mpc::Mpc& mpc);
        static void saveCurrentState(mpc::Mpc& mpc);

        // Load and save presets by filename in the default preset path.
        // See Paths::midiControlPresetsPath.
        static void saveCurrentMappingToFile(mpc::Mpc& mpc, const std::string& name, int autoloadMode);
    };
}