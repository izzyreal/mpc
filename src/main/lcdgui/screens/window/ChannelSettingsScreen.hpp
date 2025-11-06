#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::engine
{
    class IndivFxMixer;
    class StereoMixer;
} // namespace mpc::engine

namespace mpc::lcdgui::screens::window
{
    class ChannelSettingsScreen final : public ScreenComponent
    {

    public:
        ChannelSettingsScreen(Mpc &mpc, int layerIndex);

        void setNote(int newNote);

        void turnWheel(int increment) override;

        void open() override;
        void close() override;

        void update(Observable *observable, Message message) override;

    private:
        int note = 35;
        const std::vector<std::string> fxPathNames{"--", "M1", "M2", "R1",
                                                   "R2"};
        const std::vector<std::string> stereoNamesSlash{
            "-", "1/2", "1/2", "3/4", "3/4", "5/6", "5/6", "7/8", "7/8"};
        void displayNoteField();
        void displayStereoVolume();
        void displayIndividualVolume();
        void displayFxSendLevel();
        void displayPanning();
        void displayOutput();
        void displayFxPath();
        void displayFollowStereo();
        void displayChannel();

        std::shared_ptr<engine::IndivFxMixer> getIndivFxMixerChannel() const;
        std::shared_ptr<engine::StereoMixer> getStereoMixerChannel() const;
    };
} // namespace mpc::lcdgui::screens::window
