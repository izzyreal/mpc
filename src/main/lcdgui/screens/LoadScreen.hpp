#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc
{
    class Mpc;
}

namespace mpc::disk
{
    class MpcFile;
} // namespace mpc::disk

namespace mpc::lcdgui::screens
{
    class LoadScreen final : public ScreenComponent
    {
    public:
        LoadScreen(Mpc &mpc, int layerIndex);

        void function(int i) override;
        void openWindow() override;
        void turnWheel(int i) override;

        void open() override;
        void up() override;

        std::shared_ptr<disk::MpcFile> getSelectedFile() const;

        void setFileLoad(int i);
        int getFileLoad() const;

        int view = 0;
        int fileLoad = 0;

    private:
        const std::vector<std::string> views{"All Files", ".SND", ".PGM",
                                             ".APS",      ".MID", ".ALL",
                                             ".WAV",      ".SEQ", ".SET"};

        int device = 0;

        void displayView() const;
        void displayDirectory() const;
        void displayFreeSnd() const;
        void displayFile() const;
        void displaySize() const;
        void displayDevice();
        void displayDeviceType();

        void setView(int i);
        std::string getSelectedFileName() const;
        bool isSelectedFileDirectory() const;
        void setFileLoadWithMaxCheck(int i);
        unsigned long getFileSizeKb() const;

        void loadSound(bool shouldBeConverted);
    };
} // namespace mpc::lcdgui::screens
