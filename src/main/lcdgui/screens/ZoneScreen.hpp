#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::sampler
{
    class Sampler;
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

        void initZones();
        int getZoneStart(int zoneIndex) const;
        int getZoneEnd(int zoneIndex) const;
        int getZoneCount() const;
        void setZoneCount(int zoneCountToUse);
        int getSelectedZoneIndex() const;
        void setZoneStart(int zoneIndex, int start);
        void setZoneEnd(int zoneIndex, int end);

    private:
        void displayWave();
        void displaySnd() const;
        void displayPlayX() const;
        void displaySt() const;
        void displayEnd() const;
        void displayZone() const;

        const std::vector<std::string> playXNames{"ALL", "ZONE", "BEFOR ST",
                                                  "BEFOR TO", "AFTR END"};
        int zoneCount = 16;
        uint32_t numberOfFramesAtLastZoneInitialization = 0;
        std::vector<std::vector<int>> zones;
        int selectedZoneIndex = 0;

        void setSelectedZoneIndex(int zoneIndexToUse);
        std::vector<int> getZone() const;

        friend class sampler::Sampler;
    };
} // namespace mpc::lcdgui::screens
