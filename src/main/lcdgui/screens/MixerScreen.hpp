#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/MixerStrip.hpp"

#include <memory>
#include <bitset>

namespace mpc::engine
{
    class StereoMixer;
    class IndivFxMixer;
} // namespace mpc::engine

namespace mpc::lcdgui::screens
{
    class MixerScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;
        void up() override;
        void down() override;
        void left() override;
        void right() override;
        void openWindow() override;

        MixerScreen(Mpc &mpc, int layerIndex);

        void open() override;
        void close() override;

        int xPos = 0;

    private:
        void recordMixerEvent(int pad, int param, int value) const;
        void setXPos(unsigned char newXPos);

        const std::vector<std::string> fxPathNames{"--", "M1", "M2", "R1",
                                                   "R2"};
        const std::vector<std::string> stereoNames{"-",  "12", "12", "34", "34",
                                                   "56", "56", "78", "78"};
        const std::vector<std::string> monoNames{"-", "1", "2", "3", "4",
                                                 "5", "6", "7", "8"};

        int tab = 0;
        int lastTab = -1;

        std::bitset<16> selection;

        int yPos = 0;
        std::vector<std::shared_ptr<MixerStrip>> mixerStrips;
        void addMixerStrips();
        void displayMixerStrip(int stripIndex);
        void displayMixerStrips();
        void displayFunctionKeys() const;
        void displayStereoFaders();
        void displayIndivFaders();
        void displayPanning();
        void displayIndividualOutputs();
        void displayFxPaths();
        void displayFxSendLevels();

        std::shared_ptr<engine::StereoMixer>
        getStereoMixerChannel(int index) const;

        std::shared_ptr<engine::IndivFxMixer>
        getIndivFxMixerChannel(int index) const;

        bool stripHasStereoSound(int stripIndex) const;

        void setLink(bool b);
        void setTab(int i);
        void setYPos(int i);

    public:
        int getTab() const;
        void pressPadIndexWithoutBank(uint8_t padIndexWithoutBank);

        void update(Observable *o, Message message) override;
    };
} // namespace mpc::lcdgui::screens
