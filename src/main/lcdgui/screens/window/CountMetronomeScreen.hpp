#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include <vector>
#include <string>

namespace mpc::lcdgui::screens::window
{
    class CountMetronomeScreen final : public ScreenComponent
    {
        std::vector<std::string> countInNames = {"OFF", "REC ONLY", "REC+PLAY"};
        std::vector<std::string> rateNames = {"1/4",    "1/4(3)", "1/8",
                                              "1/8(3)", "1/16",   "1/16(3)",
                                              "1/32",   "1/32(3)"};

        int countIn = 1;
        std::atomic<bool> inPlay{false};
        int rate = 0;
        bool waitForKey = false;
        std::atomic<bool> inRec{true};

    public:
        int getCountInMode() const;
        bool getInPlay() const;
        int getRate() const;
        bool isWaitForKeyEnabled() const;
        bool getInRec() const;
        void setCountIn(int i);
        void setInPlay(bool b);
        void setRate(int i);
        void setWaitForKey(bool b);
        void setInRec(bool b);

    private:
        void displayWaitForKey() const;
        void displayInRec() const;
        void displayRate() const;
        void displayInPlay() const;
        void displayCountIn() const;

    public:
        CountMetronomeScreen(Mpc &mpc, int layerIndex);

        void open() override;

        void function(int i) override;
        void turnWheel(int i) override;
    };
} // namespace mpc::lcdgui::screens::window
