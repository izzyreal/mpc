#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include <lcdgui/MixerStrip.hpp>

#include <memory>

namespace mpc::lcdgui::screens {
class SelectMixerDrumScreen;
}

namespace mpc::lcdgui::screens::window {
class ChannelSettingsScreen;
}

namespace ctoot::mpc {
class MpcStereoMixerChannel;
class MpcIndivFxMixerChannel;
}

namespace mpc::lcdgui::screens
{
class MixerScreen
: public mpc::lcdgui::ScreenComponent
{
    
public:
    void function(int i) override;
    void turnWheel(int i) override;
    void up() override;
    void down() override;
    void left() override;
    void right() override;
    void openWindow() override;
    
    MixerScreen(mpc::Mpc& mpc, const int layerIndex);
    ~MixerScreen();
    
    void open() override;

private:
    void recordMixerEvent(int pad, int param, int value);
    void turnWheelLinked(int i);
    
private:
    const std::vector<std::string> fxPathNames{ "--", "M1", "M2", "R1", "R2" };
    const std::vector<std::string> stereoNames{ "-", "12", "12", "34", "34", "56", "56", "78", "78" };
    const std::vector<std::string> monoNames{ "-", "1", "2", "3", "4", "5", "6", "7", "8" };
    
private:
    int tab = 0;
    int lastTab = -1;
    bool link = false;

    int xPos = 0;
    int yPos = 0;
    std::vector<std::weak_ptr<mpc::lcdgui::MixerStrip>> mixerStrips;
    void addMixerStrips();
    void displayMixerStrip(int i);
    void displayMixerStrips();
    void displayFunctionKeys();
    void displayStereoFaders();
    void displayIndivFaders();
    void displayPanning();
    void displayIndividualOutputs();
    void displayFxPaths();
    void displayFxSendLevels();
    
    std::shared_ptr<ctoot::mpc::MpcStereoMixerChannel> getStereoMixerChannel(int index);
    
    std::shared_ptr<ctoot::mpc::MpcIndivFxMixerChannel> getIndivFxMixerChannel(int index);
    
    void setLink(bool b);
    void setTab(int i);
    void setYPos(int i);
    
public:
    int getTab();
    int getXPos();
    
public:
    void update(moduru::observer::Observable* o, nonstd::any arg) override;
    
private:
    friend class SelectMixerDrumScreen;
    friend class mpc::lcdgui::screens::window::ChannelSettingsScreen;
    
};
}
