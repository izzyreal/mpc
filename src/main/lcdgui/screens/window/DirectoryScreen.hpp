#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::disk
{
    class MpcFile;
} // namespace mpc::disk

namespace mpc::lcdgui::screens::window
{
    class DirectoryScreen final : public ScreenComponent
    {
    public:
        DirectoryScreen(Mpc &mpc, int layerIndex);

        void function(int f) override;
        void left() override;
        void right() override;
        void up() override;
        void down() override;
        void turnWheel(int i) override;
        void open() override;

        void findYOffset0();
        void setYOffset1(int i);
        int getYOffset1() const;
        std::shared_ptr<disk::MpcFile> getSelectedFile();
        int yPos0 = 0;
        int yOffset0 = 0;
        int yOffset1 = 0;

    private:
        int xPos = 0;

        void setFunctionKeys();
        std::shared_ptr<disk::MpcFile> getFileFromGrid(int x, int y) const;
        void displayLeftFields();
        void displayRightFields();
        void refreshFocus() const;

        std::vector<std::string> getFirstColumn() const;
        std::vector<std::string> getSecondColumn() const;
        int getXPos() const;
        void setYOffset0(int i);
        void setYPos0(int i);
        std::string padFileName(const std::string &s,
                                const std::string &pad) const;
        void drawGraphicsLeft();
        void drawGraphicsRight();
    };
} // namespace mpc::lcdgui::screens::window
