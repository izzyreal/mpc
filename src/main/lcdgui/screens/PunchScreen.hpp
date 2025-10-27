#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <lcdgui/screens/WithTimesAndNotes.hpp>

namespace mpc::lcdgui::screens
{

    class PunchScreen : public mpc::lcdgui::ScreenComponent,
                        private mpc::lcdgui::screens::WithTimesAndNotes
    {

    public:
        void turnWheel(int i) override;
        void function(int i) override;

        PunchScreen(mpc::Mpc &mpc, const int layerIndex);

        void open() override;

        void rec() override;
        void overDub() override;

        int getActiveTab() const;
        void setActiveTab(const int);
        const std::vector<std::string> &getTabNames() const;

    private:
        const std::vector<std::string> tabNames{"punch", "trans", "second-seq"};
        const std::vector<std::string> autoPunchNames{
            "PUNCH IN ONLY", "PUNCH OUT ONLY", "PUNCH IN OUT"};
        int tab = 0;

        void setAutoPunch(int i);

        void displayAutoPunch();
        void displayBackground();

        void displayTime() override;
        void displayNotes() override {}
    };
} // namespace mpc::lcdgui::screens
