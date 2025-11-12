#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class ZoneStartFineScreen;
    class ZoneEndFineScreen;
    class EditSoundScreen;
} // namespace mpc::lcdgui::screens::window

namespace mpc::sampler
{
    class Sampler;
}

namespace mpc::lcdgui::screens::window
{
    class NumberOfZonesScreen;
}

namespace mpc::lcdgui::screens
{

    class ZoneScreen final : public ScreenComponent
    {

    public:
        ZoneScreen(Mpc &mpc, int layerIndex);

        void open() override;
        void openWindow() override;
        void function(int f) override;
        void turnWheel(int i) override;
        void pressEnter() override;
        void left() override;
        void right() override;
        void setSlider(int) override;

        void setSliderZoneStart(int);
        void setSliderZoneEnd(int);

    private:
        void displayWave();
        void displaySnd() const;
        void displayPlayX() const;
        void displaySt() const;
        void displayEnd() const;
        void displayZone() const;

        const std::vector<std::string> playXNames{"ALL", "ZONE", "BEFOR ST",
                                                  "BEFOR TO", "AFTR END"};
        int numberOfZones = 16;
        uint32_t numberOfFramesAtLastZoneInitialization = 0;
        std::vector<std::vector<int>> zones;
        int zone = 0;

        void initZones();
        void setZoneStart(int zoneIndex, int start);
        int getZoneStart(int zoneIndex) const;
        void setZoneEnd(int zoneIndex, int end);
        int getZoneEnd(int zoneIndex) const;
        void setZone(int i);
        std::vector<int> getZone() const;

        friend class NumberOfZonesScreen;
        friend class ZoneStartFineScreen;
        friend class ZoneEndFineScreen;
        friend class EditSoundScreen;
        friend class sampler::Sampler;
    };
} // namespace mpc::lcdgui::screens
