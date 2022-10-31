#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <string>

namespace mpc::nvram { class MidiMappingPersistence; }
namespace mpc::audiomidi { class MpcMidiInput; }

namespace mpc::lcdgui::screens{

    class VmpcMidiScreen
            : public mpc::lcdgui::ScreenComponent
    {
    public:
        struct Command {
            bool isNote = false;
            int channelIndex = -2;
            int value = -2;
            bool isEmpty() {
                return !isNote && channelIndex == -2 && value == -2;
            }
            void reset() {
                isNote = false; channelIndex = -2; value = -2;
            }
            Command() {}
            Command(bool newIsNote, int newChannelIndex, int newValue) : isNote(newIsNote), channelIndex(newChannelIndex), value(newValue) {}
            Command(const Command& c) : isNote(c.isNote), channelIndex(c.channelIndex), value(c.value) {}
            bool equals(const Command& other) {
                return isNote == other.isNote && channelIndex == other.channelIndex && value == other.value;
            }
        };

        VmpcMidiScreen(mpc::Mpc&, int layerIndex);

        void open() override;
        void up() override;
        void down() override;
        void left() override;
        void right() override;
        void function(int i) override;
        void mainScreen() override;
        void turnWheel(int i) override;

        bool isLearning();
        void setLearnCandidate(const bool isNote, const char channelIndex, const char value);
        void setLabelCommand(std::string& label, Command& c);

    private:
        int row = 0;
        int rowOffset = 0;
        int column = 0;
        Command learnCandidate;

        bool learning = false;
        bool hasMappingChanged();

        std::vector<std::pair<std::string, Command>> editableLabelCommands;
        std::vector<std::pair<std::string, Command>> labelCommands;
        void setLearning(bool b);
        void updateRows();
        void displayUpAndDown();

        friend class mpc::nvram::MidiMappingPersistence;
        friend class mpc::audiomidi::MpcMidiInput;
    };
}
