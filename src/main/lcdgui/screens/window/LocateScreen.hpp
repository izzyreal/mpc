#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <tuple>
#include <vector>

namespace mpc::lcdgui::screens::window {
    class LocateScreen : public mpc::lcdgui::ScreenComponent {

    public:
        typedef std::tuple<uint16_t, uint8_t, uint8_t> Location;

        LocateScreen(mpc::Mpc& mpc, int layerIndex);

        void function(int) override;
        void turnWheel(int) override;

        void open() override;

        void setLocations(const std::vector<Location>&);
        std::vector<Location>& getLocations();

    private:
        std::vector<Location> locations = std::vector<Location>(9, {0, 0, 0});

        uint16_t barIndex = 0;
        uint8_t beatIndex = 0;
        uint8_t clock = 0;

        void displayBar();
        void displayBeat();
        void displayClock();
        void displayLocations();

        void setBarIndex(int16_t);
        void setBeatIndex(int8_t);
        void setClock(int8_t);

        uint16_t getMaxBarIndexForThisSequence();
        uint8_t getMaxBeatIndexForThisBar();
        uint8_t getMaxClockForThisBar();
    };
}