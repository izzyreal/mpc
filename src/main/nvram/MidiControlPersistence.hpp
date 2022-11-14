#pragma once

#include "Mpc.hpp"
#include "lcdgui/screens/VmpcMidiScreen.hpp"

#include <string>
#include <vector>

namespace moduru::file { class File; }

namespace mpc::nvram {

    class MidiControlPersistence
    {
    public:
        // Persistence of the in-memory mapping, so it's not required
        // to save a preset explicitly before restarting the application.
        static void restoreLastState(mpc::Mpc& mpc);
        static void saveCurrentState(mpc::Mpc& mpc);

        // Load and save presets by filename in the default preset path.
        // See Paths::midiControlPresetsPath.
        static void loadMappingFromFile(mpc::Mpc& mpc, const std::string& name);
        static void loadMappingFromFile(moduru::file::File&, std::vector<std::pair<std::string, mpc::lcdgui::screens::VmpcMidiScreen::Command>>&);
        static void saveMappingToFile(mpc::Mpc& mpc, const std::string& name);

        // Simple one-to-one mapping between file and preset name.
        // No directory structure.
        static std::vector<std::string> getAvailablePresetNames();

    private:
        static void loadMappingFromFile(mpc::Mpc& mpc, moduru::file::File&);
        static void saveMappingToFile(mpc::Mpc& mpc, moduru::file::File&);

    public:
        struct Row {

            std::string label;
            bool isNote;
            char channel;
            char value;

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

        static void loadDefaultMapping(Mpc &mpc);
    };

}