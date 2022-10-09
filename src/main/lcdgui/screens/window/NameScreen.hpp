#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <functional>

namespace mpc::controls
{
class BaseControls;
}

namespace mpc::lcdgui
{
class Underline;
}

namespace mpc::lcdgui::screens
{
class SaveScreen;
}

namespace mpc::lcdgui::screens::window
{
class EditSoundScreen;
class KeepOrRetryScreen;
class SaveAllFileScreen;
class SaveAProgramScreen;
class SaveApsFileScreen;
class SequenceScreen;
class SongWindow;
class SoundScreen;
class AutoChromaticAssignmentScreen;
class DirectoryScreen;
class MidiOutputScreen;
class TrackScreen;
class SaveASequenceScreen;
class SaveASoundScreen;
class ProgramScreen;
class VmpcDirectToDiskRecorderScreen;
}

namespace mpc::lcdgui::screens::dialog
{
class CopySoundScreen;
class StereoToMonoScreen;
class MonoToStereoScreen;
class FileExistsScreen;
class ResampleScreen;
class CreateNewProgramScreen;
}

namespace mpc::lcdgui::screens::window
{
class NameScreen
: public mpc::lcdgui::ScreenComponent
{
    
public:
    NameScreen(mpc::Mpc& mpc, const int layerIndex);
    
    void open() override;
    void close() override;
    
    void left() override;
    void right() override;
    void turnWheel(int j) override;
    void function(int i) override;
    void pressEnter() override;
    void mainScreen() override;
    void numpad() {} // Block ways to leave the screen inadvertently

    void typeCharacter(char c);
    void backSpace();

private:
    const std::vector<std::string> saveScreens = { "save-a-program", "save-a-sequence", "save-a-sound" };
    
    void drawUnderline();
    void initEditColors();
    void saveName();
    void displayName();
    std::weak_ptr<mpc::lcdgui::Underline> findUnderline();
    
    void setName(std::string name);
    void setNameLimit(int i);
    void setName(std::string str, int i);
    std::string getNameWithoutSpaces();
    void changeNameCharacter(int i, bool up);
    void setRenamerAndScreenToReturnTo(const std::function<void(std::string&)>&, const std::string&);
    
    std::function<void(std::string&)> renamer = [](std::string&){};
    std::function<void()> actionWhenGoingToMainScreen = [](){};
    std::string screenToReturnTo = "";
    
    std::string name = "";
    bool editing = false;
    std::string parameterName = "";
    int nameLimit = 0;
    std::string originalName = "";
    
    friend class mpc::controls::BaseControls;
    friend class EditSoundScreen;
    friend class KeepOrRetryScreen;
    friend class SaveAllFileScreen;
    friend class SaveAProgramScreen;
    friend class SaveApsFileScreen;
    friend class SaveASoundScreen;
    friend class VmpcDirectToDiskRecorderScreen;
    friend class mpc::lcdgui::screens::dialog::CopySoundScreen;
    friend class mpc::lcdgui::screens::dialog::StereoToMonoScreen;
    friend class mpc::lcdgui::screens::dialog::MonoToStereoScreen;
    friend class mpc::lcdgui::screens::dialog::FileExistsScreen;
    friend class mpc::lcdgui::screens::dialog::ResampleScreen;
    friend class mpc::lcdgui::screens::dialog::CreateNewProgramScreen;
    friend class SequenceScreen;
    friend class SongWindow;
    friend class SoundScreen;
    friend class AutoChromaticAssignmentScreen;
    friend class DirectoryScreen;
    friend class MidiOutputScreen;
    friend class TrackScreen;
    friend class SaveASequenceScreen;
    friend class ProgramScreen;
    friend class mpc::lcdgui::screens::SaveScreen;
};
}
