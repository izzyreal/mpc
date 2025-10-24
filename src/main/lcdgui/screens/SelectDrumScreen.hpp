#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
    class PgmAssignScreen;
    class PgmParamsScreen;
    class PurgeScreen;
    class DrumScreen;
} // namespace mpc::lcdgui::screens

namespace mpc::lcdgui::screens
{
    class SelectDrumScreen
        : public mpc::lcdgui::ScreenComponent
    {

    public:
        void function(int i) override;

        SelectDrumScreen(mpc::Mpc &mpc, const int layerIndex);

        void open() override;

    private:
        std::string redirectScreen;

        friend class PgmAssignScreen;
        friend class PgmParamsScreen;
        friend class DrumScreen;
        friend class PurgeScreen;
    };
} // namespace mpc::lcdgui::screens
