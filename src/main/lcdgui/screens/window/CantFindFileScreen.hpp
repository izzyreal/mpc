#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include <atomic>

namespace mpc::lcdgui::screens::window
{
    class CantFindFileScreen final : public ScreenComponent
    {
    public:
        void function(int i) override;
        void open() override;

        CantFindFileScreen(Mpc &mpc, int layerIndex);

        // Called on the UI thread, including when the dialog is already open.
        void setFileName(const std::string &name);
        const std::string &getFileName() const;

        // The load worker waits; the UI thread supplies the decision.
        std::atomic<bool> skipAll{false};
        std::atomic<bool> waitingForUser{false};

    private:
        std::string fileName;
    };
} // namespace mpc::lcdgui::screens::window
