#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens
{
    class SaveScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;

        SaveScreen(Mpc &mpc, int layerIndex);

        void open() override;
        void openWindow() override;
        void up() override;

        unsigned char getProgramIndex() const;

    private:
        const std::vector<std::string> types{
            u8"Save All Sequences\u00CE&\u00CESongs", "Save a Sequence",
            u8"Save All Programs\u00CE&\u00CESounds", "Save a Program & Sounds",
            "Save a Sound"};

        int type = 0;
        int device = 0;
        unsigned char programIndex = 0;
        void setType(int i);

        void displayType() const;
        void displayFile() const;
        void displaySize() const;
        void displayFree() const;
        void displayDirectory() const;
        void displayDevice();
        void displayDeviceType();
    };
} // namespace mpc::lcdgui::screens
