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

        void setNote(DrumNoteNumber);

        void turnWheel(int increment) override;

        void open() override;
        void close() override;

        void update(Observable *observable, Message message) override;

    private:
        DrumNoteNumber note = MinDrumNoteNumber;
        const std::vector<std::string> fxPathNames{"--", "M1", "M2", "R1",
                                                   "R2"};
        const std::vector<std::string> stereoNamesSlash{
            "-", "1/2", "1/2", "3/4", "3/4", "5/6", "5/6", "7/8", "7/8"};
        void displayNoteField() const;
        void displayStereoVolume() const;
        void displayIndividualVolume() const;
        void displayFxSendLevel() const;
        void displayPanning() const;
        void displayOutput() const;
        void displayFxPath() const;
        void displayFollowStereo() const;
        void displayChannel() const;

        std::shared_ptr<engine::IndivFxMixer> getIndivFxMixerChannel() const;
        std::shared_ptr<engine::StereoMixer> getStereoMixerChannel() const;
    };
} // namespace mpc::lcdgui::screens::window
