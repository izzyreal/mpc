#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens
{
    class MixerSetupScreen final : public ScreenComponent
    {

    public:
        MixerSetupScreen(Mpc &mpc, int layerIndex);
        void turnWheel(int increment) override;
        void function(int i) override;
        void open() override;

    private:
        void displayMasterLevel();
        void displayFxDrum() const;
        void displayStereoMixSource() const;
        void displayIndivFxSource() const;
        void displayCopyPgmMixToDrum() const;
        void displayRecordMixChanges() const;

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
        bool isStereoMixSourceDrum() const;
        bool isIndivFxSourceDrum() const;

    public:
        int getMasterLevel() const;
        void setMasterLevel(int i);
        int getFxDrum() const;
        void setFxDrum(int i);
        void setStereoMixSourceDrum(bool b);
        void setIndivFxSourceDrum(bool b);
        bool isCopyPgmMixToDrumEnabled() const;
        void setCopyPgmMixToDrumEnabled(bool b);
        bool isRecordMixChangesEnabled() const;
        void setRecordMixChangesEnabled(bool b);
        std::string getMasterLevelString();
    };
} // namespace mpc::lcdgui::screens
