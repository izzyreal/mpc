#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <string>

namespace mpc::nvram { class MidiMappingPersistence; }

namespace mpc::lcdgui::screens{

class VmpcMidiScreen
: public mpc::lcdgui::ScreenComponent
{
public:
    struct Command {
        bool isNote;
        int channelIndex;
        int value;
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
    void setLearnCandidate(const int rawKeyCode);
    void setLabelCommand(std::string& label, Command& c);

private:
    int row = 0;
    int rowOffset = 0;
    int column = 0;
    int learnCandidate = -1;
    
    bool learning = false;
    bool hasMappingChanged();
    
    std::vector<std::pair<std::string, Command>> labelCommands;
    void setLearning(bool b);
    void updateRows();
    void displayUpAndDown();

    friend class mpc::nvram::MidiMappingPersistence;
};
}
