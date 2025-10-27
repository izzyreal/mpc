#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::disk
{
    class ProgramLoader;
    class ApsLoader;
} // namespace mpc::disk

namespace mpc::lcdgui::screens::window
{
    class CantFindFileScreen : public mpc::lcdgui::ScreenComponent
    {
    public:
        void function(int i) override;
        void open() override;

        CantFindFileScreen(mpc::Mpc &mpc, const int layerIndex);

    private:
        std::string fileName;
        bool skipAll = false;
        bool waitingForUser = false;

        friend class mpc::disk::ProgramLoader;
        friend class mpc::disk::ApsLoader;
    };
} // namespace mpc::lcdgui::screens::window
