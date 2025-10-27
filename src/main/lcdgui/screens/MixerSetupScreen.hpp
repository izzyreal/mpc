#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
    class MixerSetupScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        MixerSetupScreen(mpc::Mpc &mpc, const int layerIndex);
        void turnWheel(int increment) override;
        void function(int i) override;
        void open() override;

    private:
        void displayMasterLevel();
        void displayFxDrum();
        void displayStereoMixSource();
        void displayIndivFxSource();
        void displayCopyPgmMixToDrum();
        void displayRecordMixChanges();

    private:
        std::vector<std::string> masterLevelNames = {u8"-\u00D9\u00DAdB",
                                                     "-72dB",
                                                     "-66dB",
                                                     "-60dB",
                                                     "-54dB",
                                                     "-48dB",
                                                     "-42dB",
                                                     "-36dB",
                                                     "-30dB",
                                                     "-24dB",
                                                     "-18dB",
                                                     "-12dB",
                                                     "-6dB",
                                                     "0dB",
                                                     "6dB",
                                                     "12dB"};
        std::vector<int8_t> masterLevelValues = {-128, -72, -66, -60, -54, -48,
                                                 -42,  -36, -30, -24, -18, -12,
                                                 -6,   0,   6,   12};
        int masterLevel = 0;
        int fxDrum = 0;
        bool stereoMixSourceDrum = false;
        bool indivFxSourceDrum = false;
        bool copyPgmMixToDrumEnabled = true;
        bool recordMixChangesEnabled = false;

    public:
        bool isStereoMixSourceDrum();
        bool isIndivFxSourceDrum();

    public:
        int getMasterLevel() const;
        void setMasterLevel(int i);
        int getFxDrum();
        void setFxDrum(int i);
        void setStereoMixSourceDrum(bool b);
        void setIndivFxSourceDrum(bool b);
        bool isCopyPgmMixToDrumEnabled();
        void setCopyPgmMixToDrumEnabled(bool b);
        bool isRecordMixChangesEnabled();
        void setRecordMixChangesEnabled(bool b);
        std::string getMasterLevelString();
    };
} // namespace mpc::lcdgui::screens
