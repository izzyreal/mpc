#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include <disk/Volume.hpp>

namespace mpc::lcdgui::screens
{
    class VmpcDisksScreen final : public ScreenComponent
    {
    public:
        VmpcDisksScreen(Mpc &mpc, int layerIndex);
        void open() override;
        void function(int i) override;
        void turnWheel(int i) override;
        void up() override;
        void down() override;

        void refreshConfig();

    private:
        std::map<std::string, disk::MountMode> config;
        int row = 0;
        int rowOffset = 0;

        void displayRows();
        void displayFunctionKeys() const;
        void displayUpAndDown();

        bool hasConfigChanged() const;
    };
} // namespace mpc::lcdgui::screens
