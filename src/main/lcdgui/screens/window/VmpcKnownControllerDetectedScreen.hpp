#pragma once

#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class VmpcKnownControllerDetectedScreen final : public ScreenComponent
    {
    public:
        VmpcKnownControllerDetectedScreen(Mpc &, int layer);

        void function(int) override;
        void open() override;

        void setControllerName(std::string);

    private:
        void displayMessage() const;

        std::string controllerName;
    };
} // namespace mpc::lcdgui::screens::window