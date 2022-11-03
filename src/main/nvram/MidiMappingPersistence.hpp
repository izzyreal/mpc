#pragma once

#include "Mpc.hpp"

#include <string>
#include <vector>

namespace moduru::file { class File; }

namespace mpc::nvram {

    class MidiMappingPersistence
    {
    public:
        // Persistence of the in-memory mapping, so it's not required
        // to save a preset explicitly before restarting the application.
        static void restoreLastState(mpc::Mpc& mpc);
        static void saveCurrentState(mpc::Mpc& mpc);

        // Load and save presets by filename in the default preset path.
        // See Paths::midiControllerPresetsPath.
        static void loadMappingFromFile(mpc::Mpc& mpc, std::string name);
        static void saveMappingToFile(mpc::Mpc& mpc, std::string name);

        // Simple one-to-one mapping between file and preset name.
        // No directory structure.
        static std::vector<std::string> getAvailablePresetNames();

    private:
        // Internal utility method to support in-memory persistence
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