#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::nvram
{
    class NvRam;
}

namespace mpc::lcdgui::screens
{
    class VmpcAutoSaveScreen final : public ScreenComponent
    {

    public:
        void turnWheel(int i) override;

        VmpcAutoSaveScreen(Mpc &mpc, int layerIndex);

        void open() override;
        void function(int) override;

    private:
        const std::vector<std::string> autoSaveOnExitNames{"Disabled",
                                                           "Enabled"};
        const std::vector<std::string> autoLoadOnStartNames{"Disabled", "Ask",
                                                            "Enabled"};

        int autoSaveOnExit = 1;
        int autoLoadOnStart = 1;

        void displayAutoSaveOnExit() const;
        void displayAutoLoadOnStart() const;

        friend class nvram::NvRam;

    public:
        void setAutoSaveOnExit(int i);
        void setAutoLoadOnStart(int i);

        int getAutoSaveOnExit() const;
        int getAutoLoadOnStart() const;
    };
} // namespace mpc::lcdgui::screens
