#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc
{
    class Mpc;
}

namespace mpc::disk
{
    class AbstractDisk;
    class StdDisk;
    class RawDisk;
    class MpcFile;
} // namespace mpc::disk

namespace mpc::lcdgui::screens::window
{
    class DirectoryScreen;
    class LoadApsFileScreen;
    class LoadASequenceFromAllScreen;
    class Mpc2000XlAllFileScreen;
    class LoadASoundScreen;
    class LoadASequenceScreen;
    class NameScreen;
    class LoadAProgramScreen;
    class VmpcConvertAndLoadWavScreen;
} // namespace mpc::lcdgui::screens::window

namespace mpc::lcdgui::screens::dialog2
{
    class DeleteAllFilesScreen;
}

namespace mpc::lcdgui::screens::dialog
{
    class DeleteFileScreen;
}

namespace mpc::lcdgui::screens
{
    class LoadScreen
        : public mpc::lcdgui::ScreenComponent
    {

    public:
        void function(int i) override;
        void openWindow() override;
        void turnWheel(int i) override;

        LoadScreen(mpc::Mpc &mpc, const int layerIndex);

        void open() override;
        void up() override;

        std::shared_ptr<mpc::disk::MpcFile> getSelectedFile();

    private:
        const std::vector<std::string> views{"All Files", ".SND", ".PGM", ".APS", ".MID", ".ALL", ".WAV", ".SEQ", ".SET"};

        int view = 0;
        int fileLoad = 0;

        int device = 0;

        void displayView();
        void displayDirectory();
        void displayFreeSnd();
        void displayFile();
        void displaySize();
        void displayDevice();
        void displayDeviceType();

        void setView(int i);
        std::string getSelectedFileName();
        bool isSelectedFileDirectory();
        void setFileLoadWithMaxCheck(int i);
        void setFileLoad(int i);

        unsigned long getFileSizeKb();

        void loadSound(bool shouldBeConverted);

    private:
        friend class mpc::lcdgui::screens::window::LoadAProgramScreen;
        friend class mpc::lcdgui::screens::window::DirectoryScreen;
        friend class mpc::lcdgui::screens::window::LoadApsFileScreen;
        friend class mpc::lcdgui::screens::window::LoadASequenceFromAllScreen;
        friend class mpc::lcdgui::screens::window::Mpc2000XlAllFileScreen;
        friend class mpc::lcdgui::screens::window::LoadASoundScreen;
        friend class mpc::lcdgui::screens::window::LoadASequenceScreen;
        friend class mpc::lcdgui::screens::window::NameScreen;
        friend class mpc::lcdgui::screens::window::VmpcConvertAndLoadWavScreen;
        friend class mpc::lcdgui::screens::dialog2::DeleteAllFilesScreen;
        friend class mpc::lcdgui::screens::dialog::DeleteFileScreen;
        friend class mpc::disk::AbstractDisk;
        friend class mpc::disk::StdDisk;
        friend class mpc::disk::RawDisk;
        friend class mpc::Mpc;
    };
} // namespace mpc::lcdgui::screens
