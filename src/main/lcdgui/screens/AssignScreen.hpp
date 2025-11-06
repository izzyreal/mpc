#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens
{
    class AssignScreen final : public ScreenComponent
    {

    public:
        AssignScreen(Mpc &mpc, int layerIndex);

    public:
        void turnWheel(int i) override;
        void open() override;
        void close() override;
        void update(Observable *o, Message message) override;

    private:
        void displayAssignNote() const;
        void displayParameter() const;
        void displayHighRange() const;
        void displayLowRange() const;
        void displayAssignNv() const;

    private:
        std::vector<std::string> typeNames = {"TUNING", "DECAY", "ATTACK",
                                              "FILTER"};
    };
} // namespace mpc::lcdgui::screens
