#include <string>
#include <utility>
#include <vector>

namespace mpc::nvram {
    struct MidiControlCommand {

        std::string label;
        bool isNote = false;
        char channel = -2;
        char value = -2;

        bool isEmpty();

        void reset();

        MidiControlCommand() = default;

        MidiControlCommand(std::string newLabel, bool newIsNote, int newChannelIndex, int newValue);

        MidiControlCommand(const MidiControlCommand &c);

        bool equals(const MidiControlCommand &other);

        std::vector<char> toBytes();
    };
}