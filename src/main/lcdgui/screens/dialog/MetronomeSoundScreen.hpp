#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include <vector>

namespace mpc::sampler
{
    class Sampler;
}

namespace mpc::disk
{
    class AllLoader;
}

namespace mpc::lcdgui
{
    class Field;
    class Label;
    class Background;
} // namespace mpc::lcdgui

namespace mpc::lcdgui::screens::dialog
{
    class MetronomeSoundScreen final : public ScreenComponent
    {

    public:
        MetronomeSoundScreen(Mpc &mpc, int layerIndex);

        void turnWheel(int notch) override;
        void function(int i) override;
        void open() override;
        void close() override;

        void update(Observable *o, Message message) override;

    private:
        std::vector<std::string> soundNames = {"CLICK", "DRUM1", "DRUM2",
                                               "DRUM3", "DRUM4"};

        void displaySound() const;
        void displayVolume() const;
        void displayOutput() const;
        void displayAccent() const;
        void displayNormal() const;
        void displayAccentVelo() const;
        void displayNormalVelo() const;

        int volume = 100;
        int output = 0;
        int sound = 0;
        int accentVelo = 127;
        int accentPad = 0;
        int normalVelo = 64;
        int normalPad = 0;

    public:
        int getVolume() const;
        void setVolume(int i);
        int getOutput() const;
        void setOutput(int i);
        int getSound() const;
        void setSound(int i);
        int getAccentPad() const;
        void setAccentPad(int i);
        int getAccentVelo() const;
        void setAccentVelo(int i);
        int getNormalPad() const;
        void setNormalPad(int i);
        int getNormalVelo() const;
        void setNormalVelo(int i);

    private:
        friend class disk::AllLoader;
    };
} // namespace mpc::lcdgui::screens::dialog
