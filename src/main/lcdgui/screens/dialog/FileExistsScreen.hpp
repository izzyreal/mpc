#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <functional>

namespace mpc::sampler { class Sound; }

namespace mpc::lcdgui::screens::dialog
{
class FileExistsScreen
: public mpc::lcdgui::ScreenComponent
{
public:
    FileExistsScreen(mpc::Mpc& mpc, const int layerIndex);
    void function(int i) override;
    void mainScreen() override;
    void numpad(int i) override {}

    void setLoadASoundCandidateAndExistingSound(
            std::shared_ptr<sampler::Sound> candidate,
            std::shared_ptr<sampler::Sound> existing);
    void setActionAfterAddingSound(std::function<void(bool)> action);

private:
    std::shared_ptr<sampler::Sound> loadASoundCandidate;
    std::shared_ptr<sampler::Sound> existingSound;
    std::function<void(bool)> actionAfterAddingSound = [](bool newSoundIsMono){};
};
}
